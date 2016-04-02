#include "Client/GUI2/Application.hpp"
#include "Client/Graphics/UtilitiesSf.hpp"
#include "Shared/Utils/Utilities.hpp"

#include <cassert>

namespace gui2
{

Ptr<Container> Container::make()
{
	Ptr<Container> widget = std::make_shared<Container>();
	widget->init();
	return widget;
}

void Container::init()
{
	myIsVertexRenderable = true;
	myAllVerticesInvalid = true;
	myVertexCountPrepend = 0;
	myVertexCountAppend = 0;
}

Container::~Container()
{
	for (auto & widget : myWidgets)
	{
		widget.widget->setParent(nullptr);
		widget.widget->onParent(nullptr);
	}
}

void Container::add(std::shared_ptr<Widget> widget)
{
	if (doAdd(widget))
		onAdd(widget);
}

void Container::remove(std::shared_ptr<Widget> widget)
{
	if (isContained(widget))
	{
		for (auto lWidget : widget->getLinkedWidgets())
		{
			remove(lWidget);
		}

		auto it = myWidgetIndexMap.find(widget);
		if (it == myWidgetIndexMap.end())
			return;

		onRemove(widget);

		// the widget's info struct and vertices are moved to the back.
		// the info is removed immediately, the vertices are cleaned up
		// on the next call to onUpdateVertexCache().
		moveWidget(it->second, myWidgets.size() - 1);
		myWidgets.pop_back();
		myWidgetIndexMap.erase(it);

		invalidateVertices();
		widget->setParent(nullptr);
		widget->onParent(std::dynamic_pointer_cast<Container>(shared_from_this()));

		for (auto * observer : myWidgetObservers)
		{
			widget->removeObserver(observer);

			std::shared_ptr<Container> cont = std::dynamic_pointer_cast<Container>(widget);
			if (cont)
			{
				cont->removeWidgetObserver(observer);
			}
		}
	}
}

bool Container::isContained(std::shared_ptr<const Widget> widget) const
{
	return myWidgetIndexMap.find(widget) != myWidgetIndexMap.end();
}

bool Container::isContained(const Widget * widget) const
{
	for (const auto & curW : myWidgets)
	{
		if (curW.widget.get() == widget)
		{
			return true;
		}
	}
	return false;
}

void Container::setFocusedWidget(std::shared_ptr<Widget> widget)
{
	if (!isManagingFocus())
	{
		if (getParent())
			getParent()->setFocusedWidget(widget);
		return;
	}

	if (isWidgetFocused(widget))
		return;

	if (!myFocusedWidget.expired())
		myFocusedWidget.lock()->onUnfocus();

	myFocusedWidget = widget;

	if (!myFocusedWidget.expired())
		myFocusedWidget.lock()->onFocus();
}

std::shared_ptr<Widget> Container::getFocusedWidget() const
{
	if (!isManagingFocus())
	{
		if (getParent())
			return getParent()->getFocusedWidget();
		else
			return nullptr;
	}

	return myFocusedWidget.lock();
}

bool Container::isWidgetFocused(std::shared_ptr<const Widget> widget) const
{
	if (!isManagingFocus())
	{
		if (getParent())
			return getParent()->isWidgetFocused(widget);
		else
			return false;
	}

	if (myFocusedWidget.expired())
		return false;

	std::shared_ptr<Widget> focused = myFocusedWidget.lock();

	if (focused->getParent() != shared_from_this())
	{
		// traverse parent tree of focused widget.
		std::shared_ptr<Widget> compWidget = focused;
		while (compWidget && compWidget != shared_from_this())
		{
			if (compWidget == widget)
				return true;

			compWidget = compWidget->getParent();
		}
		return false;
	}
	else
	{
		return focused == widget;
	}
}


void Container::invalidateWidgetVertices(std::shared_ptr<Widget> widget)
{
	auto it = myWidgetIndexMap.find(widget);
	if (it != myWidgetIndexMap.end())
	{
		myWidgets[it->second].verticesInvalid = true;
		invalidateVertices();
	}
}

void Container::invalidateAllWidgetVertices()
{
	myAllVerticesInvalid = true;
	invalidateVertices();
}

void Container::debug() const
{
	/*
	// debug output.
	int num = 0;
	for (const auto & w : myWidgets)
	{
		std::cout << (num++) << ": ";
		std::shared_ptr<TextWidget> text = std::dynamic_pointer_cast<TextWidget>(w.widget);
		if (text)
		{
			std::cout << text->getText() << std::endl;
		}
		else
		{
			std::cout << typeid(*w.widget.get()).name() << std::endl;
		}
	}

	std::cout << std::endl;
	*/
}

void Container::sendWidgetToFront(std::shared_ptr<Widget> widget)
{
	auto it = myWidgetIndexMap.find(widget);
	if (it == myWidgetIndexMap.end())
		return;

	std::size_t widgetIndex = it->second;
	int zpos = widget->getZPosition();

	// first skip linked widgets (if widget even has linked widgets).
	std::size_t widgetCount = 1;
	if (!widget->getLinkedWidgets().empty())
	{
		for (std::size_t i = widgetIndex + 1; i < myWidgets.size() && myWidgets[i].widget->getLinkParent() == widget; ++i)
		{
			widgetCount++;
		}
	}

	std::size_t searchIndex;

	// loop through widget vector starting at first past last linked widget until suitable z-position is found.
	for (searchIndex = widgetIndex + widgetCount; searchIndex < myWidgets.size() && myWidgets[searchIndex].widget->checkLinkParentRecursive(widget->getLinkParent()); ++searchIndex)
	{
		if (myWidgets[searchIndex].widget->getLinkParent() == widget->getLinkParent() &&
			myWidgets[searchIndex].widget->getZPosition() > zpos)
		{
			// reinsert widget before found position.
			moveWidget(widgetIndex, searchIndex - widgetCount);
			return;
		}
	}

	// no widget with higher z-position found? insert at end (top).
	moveWidget(widgetIndex, searchIndex - widgetCount);
}

void Container::sendWidgetToBack(std::shared_ptr<Widget> widget)
{
	auto it = myWidgetIndexMap.find(widget);
	if (it == myWidgetIndexMap.end())
		return;

	std::size_t widgetIndex = it->second;
	int zpos = widget->getZPosition();

	int searchIndex;

	// loop backwards through widget vector starting at widget to be moved until suitable z-position is found.
	for (searchIndex = widgetIndex; searchIndex >= 0 && myWidgets[searchIndex].widget->checkLinkParentRecursive(widget->getLinkParent()); --searchIndex)
	{
		if (myWidgets[searchIndex].widget->getLinkParent() == widget->getLinkParent() &&
			myWidgets[searchIndex].widget->getZPosition() < zpos)
		{
			// reinsert widget at found position.
			moveWidget(widgetIndex, searchIndex);
			return;
		}
	}

	// no widget with lower z-position found, insert at start of search range.
	moveWidget(widgetIndex, searchIndex + 1);
}

void Container::updateWidgetZPosition(std::shared_ptr<Widget> widget)
{
	auto it = myWidgetIndexMap.find(widget);
	if (it == myWidgetIndexMap.end())
		return;

	std::size_t widgetIndex = it->second;
	int zpos = widget->getZPosition();

	std::size_t insertIndex = widgetIndex;

	if (widget->hasLinkParent())
	{
		// general case for widgets with link parents.

		std::shared_ptr<Widget> parent = widget->getLinkParent();

		// find link parent.
		auto itP = myWidgetIndexMap.find(parent);
		if (itP == myWidgetIndexMap.end())
			return;

		std::size_t parentIndex = itP->second;
		std::size_t searchIndex;

		// loop through widget vector starting at one past link parent, ending at vector end or
		// at first widget that no longer matches parent.
		for (searchIndex = parentIndex + 1;
			 searchIndex < myWidgets.size() && myWidgets[searchIndex].widget->checkLinkParentRecursive(parent);
			 ++searchIndex)
		{
			if (myWidgets[searchIndex].widget->getParent() == parent && myWidgets[searchIndex].widget != widget &&
				myWidgets[searchIndex].widget->getZPosition() >= zpos)
			{
				// break to reinsert widget before found position.
				break;
			}
		}

		// if nothing was found, searchIndex will be end of searching range, so widget is inserted last.
		insertIndex = searchIndex;
	}
	else
	{
		// special case for unlinked widgets.

		std::size_t searchIndex;

		// loop through widget vector until suitable z-position is found.
		for (searchIndex = 0; searchIndex < myWidgets.size(); ++searchIndex)
		{
			// ignore the widget we're trying to sort or different linked widgets, and compare z-positions.
			if (!myWidgets[searchIndex].widget->hasLinkParent() && myWidgets[searchIndex].widget != widget &&
				myWidgets[searchIndex].widget->getZPosition() >= zpos)
			{
				// break to reinsert widget before found position.
				break;
			}
		}

		// if nothing was found, searchIndex will be widget vector size, so widget is inserted at the end.
		insertIndex = searchIndex;

		/*
		// find suitable widget position.
		std::size_t index = std::distance(myWidgets.begin(), std::lower_bound(myWidgets.begin(), myWidgets.end(), zpos, ZPosCompare()));

		// move widget to take place of found position.
		moveWidget(widgetIndex, index > widgetIndex ? index - 1 : index);
		*/
	}

	moveWidget(widgetIndex, insertIndex > widgetIndex ? insertIndex - 1 : insertIndex);

	// send the widget to the topmost position within its z-order.
	sendWidgetToFront(widget);
}

bool Container::isVertexRenderable() const
{
	return false;
	//return myIsVertexRenderable;
}

sf::Transform Container::getContainerTransform() const
{
	return sf::Transform::Identity;
}

sf::FloatRect Container::getContainerBoundingBox() const
{
	//return getContainerTransform().transformRect(getBaseRect());
	return getContainerTransform().getInverse().transformRect(getBaseRect());
}

sf::FloatRect Container::getContainerClipBox() const
{
	//return getContainerTransform().transformRect(getBaseRect());
	return getBaseRect();
}

bool Container::isClippingWidgets() const
{
	return true;
}

bool Container::isManagingFocus() const
{
	return true;
}

void Container::addWidgetObserver(Observer* observer)
{
	if (!isWidgetObserver(observer))
	{
		myWidgetObservers.insert(observer);
		for (auto & widget : myWidgets)
		{
			widget.widget->addObserver(observer);

			std::shared_ptr<Container> cont = std::dynamic_pointer_cast<Container>(widget.widget);
			if (cont)
			{
				cont->addWidgetObserver(observer);
			}
		}
	}
}

void Container::removeWidgetObserver(Observer* observer)
{
	if (myWidgetObservers.erase(observer))
	{
		for (auto & widget : myWidgets)
		{
			widget.widget->removeObserver(observer);

			std::shared_ptr<Container> cont = std::dynamic_pointer_cast<Container>(widget.widget);
			if (cont)
			{
				cont->removeWidgetObserver(observer);
			}
		}
	}
}

bool Container::isWidgetObserver(Observer* observer) const
{
	return myWidgetObservers.count(observer);
}

bool Container::doAdd(std::shared_ptr<Widget> widget)
{
	if (!isContained(widget))
	{
		if (widget->getParent())
			widget->getParent()->remove(widget);

		for (auto * observer : myWidgetObservers)
		{
			widget->addObserver(observer);

			std::shared_ptr<Container> cont = std::dynamic_pointer_cast<Container>(widget);
			if (cont)
			{
				cont->addWidgetObserver(observer);
			}
		}

		widget->setParent(std::dynamic_pointer_cast<Container>(shared_from_this()));

		myWidgetIndexMap[widget] = myWidgets.size();
		myWidgets.push_back(widget);

		//myIsVertexRenderable = false;

		updateWidgetZPosition(widget);

		invalidateVertices();
		widget->onParent(nullptr);

		for (auto lWidget : widget->getLinkedWidgets())
		{
			add(lWidget);
		}

		return true;
	}

	return false;
}

void Container::onUpdateFonts()
{
	for (auto & widget : myWidgets)
	{
		widget.widget->onUpdateFonts();
	}
}

void Container::onParent(std::shared_ptr<Container> oldParent)
{
	if (getParent())
	{
		if (oldParent)
		{
			// check if container got moved to different interface.
			if (oldParent->getParentInterface() != getParentInterface())
			{
				onUpdateFonts();
			}
		}
		else
		{
			onUpdateFonts();
		}
	}
}

void Container::onAdd(std::shared_ptr<Widget> widget)
{
	// to be implemented by subclass.
}

void Container::onRemove(std::shared_ptr<Widget> widget)
{
	// to be implemented by subclass.
}

void Container::onProcessContainer(WidgetEvents & events)
{
	// to be implemented by subclass.
}

void Container::onProcess(const WidgetEvents & events)
{
	sf::Transform conTransform = events.transform * getContainerTransform();

	// copy events to allow modification by container.
	WidgetEvents cEvents(events);

	// perform container subclass processing.
	onProcessContainer(cEvents);

	// check if mouse cursor is within container bounding box (mouse focus).
	if (!getContainerBoundingBox().contains(conTransform.getInverse() * cEvents.conEvents.mousePosition))
		cEvents.mouseFocus = false;

	// check if this container is currently focused (keyboard focus).
	if (getParent() && !isFocused())
		cEvents.keyboardFocus = false;

	// keep track of whether the mouse is over any widget.
	bool widgetMouseOver = false;

	// check if the left mouse button was just pressed.
	bool mousePressed = events.pressedInputs.contains(sf::Mouse::Left);

	// temporary copy of widget vector, for when widgets change their order.
	auto widgetVec = myWidgets;

	// process widgets in reverse rendering order.
	for (auto it = widgetVec.rbegin(); it != widgetVec.rend(); ++it)
	{
		Widget & widget = *it->widget;

		widget.process(WidgetEvents(cEvents.conEvents, conTransform * widget.getTransform(), cEvents.mouseFocus && widget.isVisible(), cEvents.keyboardFocus && widget.isVisible()));

		// only allow the topmost widget to respond to mouse events.
		if (widget.isVisible() && widget.isMouseOver())
		{
			cEvents.mouseFocus = false;
			widgetMouseOver = true;

			if (mousePressed)
			{
				widget.acquireFocus();

				// send link parents to front recursively.
				std::shared_ptr<Widget> linkChain = it->widget;
				while (linkChain)
				{
					linkChain->sendToFront();
					linkChain = linkChain->getLinkParent();
				}
			}
		}
	}

	// unfocus widget if mouse button 1 was pressed on the container background.
	if (cEvents.mouseFocus && mousePressed && !widgetMouseOver)
	{
		setFocusedWidget(nullptr);
	}
}

void Container::onDraw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	bool needWidgetTransform = true;
	sf::RenderStates widgetStates;

	// compute transformed clipping rectangle if necessary.
	sf::FloatRect clipRect;

	if (isClippingWidgets())
	{
		sf::Transform clipTransform = states.transform;
		//clipTransform *= getContainerTransform();
		clipRect = clipTransform.transformRect(getContainerClipBox());
	}

	// vertex cache start position. already add pre-render vertices.
	std::size_t vertBegin = 0, vertEnd = myVertexCountPrepend;

	// render widgets.
	for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
	{
		Widget & widget = *it->widget;

		// widget is vertex-renderable and has a vertex cache entry.
		if (widget.isVertexRenderable())
		{
			vertEnd += it->cachedVertexCount;
		}
		else if (widget.isVisible())
		{
			// draw everything up until the current vertex cache position.
			if (vertBegin < vertEnd)
				target.draw(myVertexCache.data() + vertBegin, vertEnd - vertBegin, sf::Triangles, states);

			vertBegin = vertEnd;

			// compute widget transformation.
			if (needWidgetTransform)
			{
				widgetStates = states;
				widgetStates.transform *= getContainerTransform();
				needWidgetTransform = false;
			}
			// draw widget.
			if (isClippingWidgets())
				drawClipped(widget, target, widgetStates, clipRect);
			else
				target.draw(widget, widgetStates);
		}
	}

	// add post-render vertices.
	vertEnd += myVertexCountAppend;

	// draw any remaining vertices.
	target.draw(myVertexCache.data() + vertBegin, vertEnd - vertBegin, sf::Triangles, states);
}

void Container::onUpdateVertexCachePrepend()
{

}

void Container::onUpdateVertexCacheAppend()
{

}

void Container::onUpdateVertexCache()
{
	// update vertices if necessary/possible.
	for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
	{
		it->widget->updateVertexCache();
	}

	myIsVertexRenderable = true;

	// get cache size before pre-render vertices.
	std::size_t oldCacheSizeDeco = myVertexCache.size();

	// append pre-render vertices.
	onUpdateVertexCachePrepend();

	// above function should never remove vertices.
	assert(myVertexCache.size() >= oldCacheSizeDeco);

	// get number of vertices added.
	std::size_t newDecoVertexCount = myVertexCache.size() - oldCacheSizeDeco;

	// compute difference.
	int cacheSizeDiffDeco = newDecoVertexCount - myVertexCountPrepend;

	// resize cache as needed.
	if (cacheSizeDiffDeco > 0)
		myVertexCache.insert(myVertexCache.begin(), cacheSizeDiffDeco, sf::Vertex());
	else if (cacheSizeDiffDeco < 0)
		myVertexCache.erase(myVertexCache.begin(), myVertexCache.begin() - cacheSizeDiffDeco);

	// copy vertices to correct position.
	std::memcpy(myVertexCache.data(), myVertexCache.data() + oldCacheSizeDeco + cacheSizeDiffDeco, newDecoVertexCount * sizeof(sf::Vertex));

	// remove vertices from end of cache.
	myVertexCache.erase(myVertexCache.end() - newDecoVertexCount, myVertexCache.end());

	// set new vertex count.
	myVertexCountPrepend = newDecoVertexCount;

	// begin processing widget vertices.
	std::size_t cachePos = myVertexCountPrepend;

	// fetch vertices from vertex-renderable widgets.
	for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
	{
		Widget & widget = *it->widget;

		// get vertices if possible.
		if (!widget.isVertexRenderable())
		{
			// mark whole container as complex-rendered.
			myIsVertexRenderable = false;
		}

		// also check if this widget needs updating.
		if (myAllVerticesInvalid || it->verticesInvalid)
		{
			// first save old vertex cache size.
			std::size_t oldCacheSize = myVertexCache.size();

			// update vertices if visible, erase if invisible or complex-rendered.
			if (widget.isVisible() && widget.isVertexRenderable())
			{
				// buffer for widget vertices.
				std::vector<sf::Vertex> vertexBuffer(widget.getVertices().size());

				// fill buffer with vertices quickly.
				std::memcpy(vertexBuffer.data(), widget.getVertices().data(), widget.getVertices().size() * sizeof(sf::Vertex));

				// transform vertices.
				for (auto itv = vertexBuffer.begin(); itv != vertexBuffer.begin() + vertexBuffer.size(); ++itv)
					itv->position = (getContainerTransform() * widget.getTransform()) * itv->position;

				// clip vertices.
				if (isClippingWidgets())
					clipVertices(vertexBuffer, 0, vertexBuffer.size(), getContainerClipBox());

				// compute needed cache size difference to fit widget vertices.
				int cacheSizeDiff = vertexBuffer.size() - it->cachedVertexCount;

				/*
				if (cacheSizeDiff != 0)
				{
					std::cout << typeid(*this).name() << " changing cache size by " << cacheSizeDiff << std::endl;
					std::cout << "widget: " << typeid(widget).name() << std::endl;
					std::shared_ptr<TextWidget> textW = std::dynamic_pointer_cast<TextWidget>(it->widget);
					if (textW) std::cout << "text: " << textW->getText() << std::endl;
					std::cout << "old vertex count: " << it->cachedVertexCount << "; new: " << widget.getVertices().size() << std::endl;
					std::cout << "current cache size: " << myVertexCache.size() << std::endl;
				}
				else
				{
					std::shared_ptr<TextWidget> textW = std::dynamic_pointer_cast<TextWidget>(it->widget);
					if (textW) std::cout << "updatingT: " << textW->getText() << std::endl;
					else std::cout << "updatingW: " << typeid(widget).name() << std::endl;
				}
				*/

				// resize cache as needed.
				if (cacheSizeDiff > 0)
					myVertexCache.insert(myVertexCache.begin() + cachePos, cacheSizeDiff, sf::Vertex());
				else if (cacheSizeDiff < 0)
					myVertexCache.erase(myVertexCache.begin() + cachePos, myVertexCache.begin() + cachePos - cacheSizeDiff);

				// copy vertices into vector quickly.
				std::memcpy(myVertexCache.data() + cachePos, vertexBuffer.data(), vertexBuffer.size() * sizeof(sf::Vertex));
			}
			else
			{
				// erase vertices.
				myVertexCache.erase(myVertexCache.begin() + cachePos, myVertexCache.begin() + cachePos + it->cachedVertexCount);
			}

			// now compute new vertex count for this particular widget.
			it->cachedVertexCount += myVertexCache.size() - oldCacheSize;

			// mark vertices as valid.
			it->verticesInvalid = false;
		}

		// increment cache position.
		cachePos += it->cachedVertexCount;
	}

	// reset all invalid variable.
	myAllVerticesInvalid = false;

	// remove any remaining vertices from deleted widgets, as well as appended vertices.
	myVertexCache.erase(myVertexCache.begin() + cachePos, myVertexCache.end());

	// keep track of vertex count before post-render call, only for final assertion.
	oldCacheSizeDeco = myVertexCache.size();

	// re-add post-render vertices, no further special actions necessary.
	onUpdateVertexCacheAppend();

	// the above function should never remove vertices.
	assert(myVertexCache.size() >= oldCacheSizeDeco);

	// calculate appended vertex count.
	myVertexCountAppend = myVertexCache.size() - cachePos;
}

void Container::onMouseOver()
{

}

void Container::onMouseAway()
{

}

void Container::onMouseDown(sf::Vector2f pos, Input button)
{

}

void Container::onMouseUp(sf::Vector2f pos, Input button)
{

}

void Container::onResize()
{
	invalidateAllWidgetVertices();
	notify(NotifyContainerBox);
}

std::shared_ptr<Interface> Container::onGetParentInterface() const
{
	return myParentInterface.lock();
}

/*
std::shared_ptr<const btx::Font> Container::onGetFont(int index) const
{
	if (getParent())
		return getParent()->getFont();
	else if (getParentInterface())
		return getParentInterface()->getParentApplication().getFont(index);
	else
		return nullptr;
}
*/

void Container::setParentInterface(std::shared_ptr<Interface> parent)
{
	if (myParentInterface.lock() != parent)
	{
		myParentInterface = parent;
		onUpdateFonts();
	}
}

void Container::moveWidget(std::size_t sourcePos, std::size_t destPos)
{
	// initial checks.
	if (sourcePos == destPos || sourcePos >= myWidgets.size() || destPos >= myWidgets.size())
		return;

	// determine range to move.
	std::size_t sourceEnd = sourcePos + 1;

	// if widget to be moved has linked widgets...
	if (!myWidgets[sourcePos].widget->getLinkedWidgets().empty())
	{
		// find first widget that is not linked to this widget.
		while (sourceEnd < myWidgets.size() && myWidgets[sourceEnd].widget->checkLinkParentRecursive(myWidgets[sourcePos].widget))
		{
			++sourceEnd;
		}
	}

	// number of widgets that are moved.
	std::size_t moveWidgetCount = sourceEnd - sourcePos;

	// determine destination range.
	std::size_t destEnd = destPos + moveWidgetCount;

	// check if move is invalid (destination within move range or ).
	if (/*(destPos > sourcePos && destPos < sourceEnd) || */destEnd > myWidgets.size())
	{
		/*std::cout << "Invalid widget move from [" << sourcePos << ";" << sourceEnd <<
					  ") to [" << destPos << ";" << destEnd << ") in container with " <<
					  myWidgets.size() << " widgets." << std::endl;*/
		return;
	}

	// update index map first.
	if (sourcePos < destPos)
	{
		// forward.
		for (std::size_t i = sourceEnd; i < destEnd; ++i)
		{
			myWidgetIndexMap[myWidgets[i].widget] -= moveWidgetCount;
		}
	}
	else
	{
		// backward.
		for (int i = sourcePos - 1; i >= (int)destPos; --i)
		{
			myWidgetIndexMap[myWidgets[i].widget] += moveWidgetCount;
		}
	}

	// update index map for moved widgets.
	for (std::size_t i = 0; i < moveWidgetCount; ++i)
		myWidgetIndexMap[myWidgets[sourcePos + i].widget] = destPos + i;

	// find vertex cache beginning for the source range.
	std::size_t vertexSourceBegin = 0;
	for (std::size_t i = 0; i < sourcePos; ++i)
	{
		vertexSourceBegin += myWidgets[i].cachedVertexCount;
	}

	// find vertex cache end for the source range.
	std::size_t vertexSourceEnd = vertexSourceBegin;
	for (std::size_t i = sourcePos; i < sourceEnd; ++i)
	{
		vertexSourceEnd += myWidgets[i].cachedVertexCount;
	}

	// find vertex cache beginning for the destination range.
	std::size_t vertexDestBegin = 0;
	if (sourceEnd < destPos)
	{
		// moving forward in non-overlapping regions: count from source end.
		vertexDestBegin = vertexSourceEnd;
		for (std::size_t i = sourceEnd; i < destPos; ++i)
		{
			vertexDestBegin += myWidgets[i].cachedVertexCount;
		}
	}
	else
	{
		// moving backward or regions overlap: count from start.
		for (std::size_t i = 0; i < destPos; ++i)
		{
			vertexDestBegin += myWidgets[i].cachedVertexCount;
		}
	}

	// find vertex cache end for the destination range.
	std::size_t vertexDestEnd = vertexDestBegin;
	for (std::size_t i = destPos; i < destEnd; ++i)
	{
		vertexDestEnd += myWidgets[i].cachedVertexCount;
	}


	// these vertex cache operations are OK to do here because the vertex cache remains the same size.

	// get iterator to start of actual widget vertices.
	auto vertexCacheStart (myVertexCache.begin() + myVertexCountPrepend);

	// move vertices in vertex cache.
	if (sourcePos < destPos)
	{
		// moving forward: source end should be new first.
		std::rotate(vertexCacheStart + vertexSourceBegin, vertexCacheStart + vertexSourceEnd, vertexCacheStart + vertexDestEnd);
	}
	else
	{
		// moving backward: source begin should be new first.
		std::rotate(vertexCacheStart + vertexDestBegin, vertexCacheStart + vertexSourceBegin, vertexCacheStart + vertexSourceEnd);
	}

	/*
	// grab the widget's cached vertices.
	std::vector<sf::Vertex> vertices(myVertexCache.begin() + vertexSourceBegin, myVertexCache.begin() + vertexSourceEnd);

	// remove vertices from old position.
	myVertexCache.erase(myVertexCache.begin() + vertexSourceBegin, myVertexCache.begin() + vertexSourceEnd);

	// insert vertices at new position.
	myVertexCache.insert(myVertexCache.begin() + vertexDestIndex, vertices.begin(), vertices.end());
	*/


	// move widget data.
	if (sourcePos < destPos)
	{
		// moving forward: source end should be new first.
		std::rotate(myWidgets.begin() + sourcePos, myWidgets.begin() + sourceEnd, myWidgets.begin() + destEnd);
	}
	else
	{
		// moving backward: source begin should be new first.
		std::rotate(myWidgets.begin() + destPos, myWidgets.begin() + sourcePos, myWidgets.begin() + sourceEnd);
	}

	/*
	// get widget info structs.
	std::vector<WidgetInfo> widgets(myWidgets.begin() + sourcePos, myWidgets.begin() + sourceEnd);

	// remove widgets from old position in vector.
	myWidgets.erase(myWidgets.begin() + sourcePos, myWidgets.begin() + sourceEnd);

	// place widgets in new position in vector.
	myWidgets.insert(myWidgets.begin() + destPos, widgets.begin(), widgets.end());
	*/

	/*
	// sanity check.
	for (std::size_t i = 0; i < myWidgets.size(); ++i)
	{
		if (myWidgetIndexMap[myWidgets[i].widget] != i)
			std::cout << "fuck: " << myWidgetIndexMap[myWidgets[i].widget] << " != " << i << std::endl;
	}
	*/
}


} // namespace gui2
