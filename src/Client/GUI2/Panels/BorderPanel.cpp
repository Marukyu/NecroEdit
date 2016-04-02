#include <Client/Game/Input.hpp>
#include <Client/GUI2/GUI.hpp>
#include <Client/GUI2/Internal/WidgetEvents.hpp>
#include <Client/GUI2/Panels/BorderPanel.hpp>
#include <Client/GUI2/Widget.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Utils/MiscMath.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace gui2
{

Ptr<BorderPanel> BorderPanel::make()
{
	Ptr<BorderPanel> widget = std::make_shared<BorderPanel>();
	widget->init();
	return widget;
}

void BorderPanel::init()
{
	Panel::init();

	myPlacements.resize(OrientationCount);
	mySideSettings.resize(OrientationCount);
}

void BorderPanel::add(std::shared_ptr<Widget> widget, Orientation orientation)
{
	if (doAdd(widget))
		place(widget, orientation);
}

void BorderPanel::add(std::shared_ptr<Widget> widget, Orientation orientation, float size)
{
	if (doAdd(widget))
		place(widget, orientation, size);
}

void BorderPanel::addPos(std::shared_ptr<Widget> widget, Orientation orientation, std::size_t index)
{
	if (doAdd(widget))
		placePos(widget, orientation, index);
}

void BorderPanel::addPos(std::shared_ptr<Widget> widget, Orientation orientation, float size, std::size_t index)
{
	if (doAdd(widget))
		placePos(widget, orientation, size, index);
}

void BorderPanel::place(std::shared_ptr<Widget> widget, Orientation orientation)
{
	place(widget, orientation, -1.f);
}

void BorderPanel::place(std::shared_ptr<Widget> widget, Orientation orientation, float size)
{
	if (!widget || widget->hasLinkParent())
		return;

	if (orientation >= OrientationCount)
		orientation = Center;

	removePlacementEntry(widget);

	placePos(widget, orientation, size, myPlacements[orientation].size());
}

void BorderPanel::placePos(std::shared_ptr<Widget> widget, Orientation orientation, std::size_t index)
{
	placePos(widget, orientation, -1.f, myPlacements[orientation].size());
}

void BorderPanel::placePos(std::shared_ptr<Widget> widget, Orientation orientation, float size, std::size_t index)
{
	if (!widget || widget->hasLinkParent())
		return;

	if (orientation >= OrientationCount)
		orientation = Center;

	removePlacementEntry(widget);

	if (index > myPlacements[orientation].size())
		index = myPlacements[orientation].size();

	bool autoSize = size < 0.f;

	widget->setAutoManagingSize(false);

	if (orientation != Center)
	{
		if (autoSize)
		{
			size = (orientation == Left || orientation == Right ? widget->getAutomaticSize().x : widget->getAutomaticSize().y);
			widget->addObserver(this);
		}
	}

	size += getPadding() * 2.f;
	mySideSettings[orientation].size += size;

	PanelData data;
	data.widget = widget;
	data.size = size;
	myPlacements[orientation].insert(myPlacements[orientation].begin() + index, data);

	//update(orientation, index);

	// update entire side.
	update(orientation, 0);
}

void BorderPanel::updateAutoSize(Orientation orientation, std::size_t index)
{
	mySideSettings[orientation].size -= myPlacements[orientation][index].size;
	myPlacements[orientation][index].size = (orientation == Left || orientation == Right ?
											 myPlacements[orientation][index].widget.lock()->getAutomaticSize().x :
											 myPlacements[orientation][index].widget.lock()->getAutomaticSize().y) + 2.f * getPadding();
	mySideSettings[orientation].size += myPlacements[orientation][index].size;
}

bool BorderPanel::update(std::shared_ptr<Widget> widget)
{
	if (!widget || !isContained(widget))
		return false;

	auto result = retrieve(widget.get());
	if (result.first != OrientationCount)
	{
		update(result.first, result.second);
	}

	return true;
}

bool BorderPanel::update(Orientation orientation, std::size_t index)
{
	if (orientation == OrientationCount)
		return false;

	if (index >= myPlacements[orientation].size())
	{
		// update resize handle.
		updateResizeHandle(orientation);

		// update dependent sides.
		switch (orientation)
		{
		case Top:
		case Bottom:
			update(Left, 0);
			update(Right, 0);
			return true;

		case Left:
		case Right:
			update(Center, 0);
			return true;

		case Center:
		default:
			return true;
		}
	}

	updateSingle(orientation, index);
	update(orientation, index + 1);

	return true;
}

void BorderPanel::setResizable(Orientation side, bool resizable)
{
	if (side < OrientationCount && mySideSettings[side].resizable != resizable)
	{
		mySideSettings[side].resizable = resizable;

		if (resizable)
		{
			mySideSettings[side].resizeHandle = ResizeHandle::make();
			link(mySideSettings[side].resizeHandle);
			mySideSettings[side].resizeHandle->setParentPanel(std::dynamic_pointer_cast<BorderPanel>(shared_from_this()));
			mySideSettings[side].resizeHandle->setSide(side);
		}
		else
		{
			unlink(mySideSettings[side].resizeHandle);
			mySideSettings[side].resizeHandle = nullptr;
		}

		update(side, 0);
	}
}

bool BorderPanel::isResizable(Orientation side) const
{
	if (side < OrientationCount)
		return mySideSettings[side].resizable;
	else
		return false;
}

void BorderPanel::setSideSize(Orientation side, float sideSize)
{
	if (getMaximumSize(side) < 0.f)
	{
		sideSize = std::max(getMinimumSize(side), sideSize);
	}
	else
	{
		sideSize = clamp(getMinimumSize(side), sideSize, getMaximumSize(side));
	}
	
	if (side < OrientationCount)
	{
		if (mySideSettings[side].size != 0.f)
		{
			float scale = sideSize / mySideSettings[side].size;
			if (mySideSettings[side].scale != scale)
			{
				mySideSettings[side].scale = scale;
				update(side, 0);
			}
		}
	}
}

float BorderPanel::getSideSize(Orientation side) const
{
	if (side < OrientationCount)
		return mySideSettings[side].size * mySideSettings[side].scale;
	else
		return 0.f;
}

void BorderPanel::setMinimumSize(Orientation side, float minSize)
{
	if (side < OrientationCount && mySideSettings[side].minSize != minSize)
	{
		mySideSettings[side].minSize = minSize;
		update(side, 0);
	}
}

float BorderPanel::getMinimumSize(Orientation side) const
{
	if (side < OrientationCount)
		return mySideSettings[side].minSize;
	else
		return 0.f;
}

void BorderPanel::setMaximumSize(Orientation side, float maxSize)
{
	if (side < OrientationCount && mySideSettings[side].maxSize != maxSize)
	{
		mySideSettings[side].maxSize = maxSize;
		update(side, 0);
	}
}

float BorderPanel::getMaximumSize(Orientation side) const
{
	if (side < OrientationCount)
		return mySideSettings[side].maxSize;
	else
		return 0.f;
}

Ptr<BorderPanel::ResizeHandle> BorderPanel::ResizeHandle::make()
{
	return std::make_shared<ResizeHandle>();
}

void BorderPanel::ResizeHandle::setParentPanel(std::shared_ptr<BorderPanel> parent)
{
	myParentPanel = parent;
}

void BorderPanel::ResizeHandle::setSide(Orientation side)
{
	mySide = side;
}

void BorderPanel::ResizeHandle::onUpdateVertexCache()
{
	clearVertices();

	//bool horiz = (getSize().x < getSize().y);

	BoxStyle boxStyle;
	//boxStyle.borderThickness = isMouseOver() ? 1.f : -1.f;
	boxStyle.borderThickness = isMouseOver() || isMouseDown() ? -1.f : -2.f;
	//boxStyle.outlineColor.a *= 0.5f;
	boxStyle.hovered = isMouseOver() || isMouseDown();
	boxStyle.activated = isMouseDown();
	vertexAddBox(BoxTypeBasic, boxStyle);
}

void BorderPanel::ResizeHandle::onProcess(const WidgetEvents & events)
{
	if (isMouseDown() && !myParentPanel.expired())
	{
		BorderPanel & panel = *myParentPanel.lock();

		float size = panel.getSideSize(mySide);

		switch (mySide)
		{
		case Top:
			size = -(myClickOffset.y - getPosition().y - events.mousePosition.y) + myInitialSize;
			break;

		case Bottom:
			size = myClickOffset.y - getPosition().y - events.mousePosition.y + myInitialSize;
			break;

		case Left:
			size = -(myClickOffset.x - getPosition().x - events.mousePosition.x) + myInitialSize;
			break;

		case Right:
			size = myClickOffset.x - getPosition().x - events.mousePosition.x + myInitialSize;
			break;

		case Center:
		default:
			break;
		}

		panel.setSideSize(mySide, size);
	}
}

void BorderPanel::ResizeHandle::onMouseDown(sf::Vector2f pos, Input button)
{
	myClickOffset = getPosition() + pos;
	if (!myParentPanel.expired())
		myInitialSize = myParentPanel.lock()->getSideSize(mySide);

	invalidateVertices();
}

bool BorderPanel::updateSingle(Orientation orientation, std::size_t index)
{
	if (orientation == OrientationCount || index >= myPlacements[orientation].size())
		return false;

	if (myPlacements[orientation][index].widget.expired())
		return false;

	Widget & widget = *myPlacements[orientation][index].widget.lock();

	if (widget.hasLinkParent())
		return false;

	float pos = 0.f;
	float size = myPlacements[orientation][index].size;

	if (orientation != Center)
	{
		for (std::size_t i = 0; i < index; ++i)
		{
			pos += myPlacements[orientation][i].size;
		}
	}
	
	pos *= mySideSettings[orientation].scale;
	size *= mySideSettings[orientation].scale;

	sf::Vector2f outerSize(getSize().x - getMargin() * 2.f, getSize().y - getMargin() * 2.f);
	sf::Vector2f widgetPos(0,0), widgetSize(outerSize);

	switch (orientation)
	{
	case Top:
		{
			widgetSize = sf::Vector2f(outerSize.x, size);
			widgetPos = sf::Vector2f(0.f, pos);
		}
		break;

	case Bottom:
		{
			widgetSize = sf::Vector2f(outerSize.x, size);
			widgetPos = sf::Vector2f(0.f, outerSize.y - pos - widgetSize.y);
		}
		break;

	case Left:
		{
			float topOff = getEffectiveSideSize(Top);
			float bottomOff = getEffectiveSideSize(Bottom);
			widgetSize = sf::Vector2f(size, outerSize.y - topOff - bottomOff);
			widgetPos = sf::Vector2f(pos, topOff);
		}
		break;

	case Right:
		{
			float topOff = getEffectiveSideSize(Top);
			float bottomOff = getEffectiveSideSize(Bottom);
			widgetSize = sf::Vector2f(size, outerSize.y - topOff - bottomOff);
			widgetPos = sf::Vector2f(outerSize.x - pos - widgetSize.x, topOff);
		}
		break;

	case Center:
	default:
		{
			float topOff = getEffectiveSideSize(Top);
			float bottomOff = getEffectiveSideSize(Bottom);
			float leftOff = getEffectiveSideSize(Left);
			float rightOff = getEffectiveSideSize(Right);
			widgetSize = sf::Vector2f(outerSize.x - leftOff - rightOff, outerSize.y - topOff - bottomOff);
			widgetPos = sf::Vector2f(leftOff, topOff);
		}
		break;
	}

	sf::Vector2f fPos(widgetPos.x + getPadding() + getMargin(), widgetPos.y + getPadding() + getMargin());
	sf::Vector2f fSize(widgetSize.x - getPadding() * 2.f, widgetSize.y - getPadding() * 2.f);

	if (isPixelPerfect())
	{
		widget.setPosition(sf::Vector2f(sf::Vector2i(fPos + sf::Vector2f(0.5f, 0.5f))));
		//widget.setSize(sf::Vector2f(sf::Vector2i(fSize + sf::Vector2f(0.5f, 0.5f))));
	}
	else
	{
		widget.setPosition(fPos);
	}
	widget.setSize(fSize);

	return true;
}

void BorderPanel::updateResizeHandle(Orientation side)
{
	if (side < OrientationCount && mySideSettings[side].resizable && mySideSettings[side].resizeHandle)
	{
		float handleSize = getEffectiveSideSize(side) - getSideSize(side);

		switch (side)
		{
		case Top:
			mySideSettings[Top].resizeHandle->setRect(getMargin() + getPadding(), getSideSize(Top) + getMargin(),
													  getSize().x - getMargin() * 2.f - getPadding() * 2.f, handleSize);
			break;

		case Bottom:
			mySideSettings[Bottom].resizeHandle->setRect(getMargin() + getPadding(), getSize().y - getSideSize(Bottom) - getMargin() - handleSize,
														 getSize().x - getMargin() * 2.f - getPadding() * 2.f, handleSize);
			break;

		case Left:
			mySideSettings[Left].resizeHandle->setRect(getSideSize(Left) + getMargin(), getEffectiveSideSize(Top) + getMargin() + getPadding(),
													   handleSize, getSize().y - getEffectiveSideSize(Top) - getEffectiveSideSize(Bottom) - getMargin() * 2.f - getPadding() * 2.f);
			break;

		case Right:
			mySideSettings[Right].resizeHandle->setRect(getSize().x - getSideSize(Right) - getMargin() - handleSize, getEffectiveSideSize(Top) + getMargin() + getPadding(),
														handleSize, getSize().y - getEffectiveSideSize(Top) - getEffectiveSideSize(Bottom) - getMargin() * 2.f - getPadding() * 2.f);
			break;

		case Center:
		default:
			break;
		}
	}
}

void BorderPanel::onUpdatePanel()
{
	for (int orient = OrientationCount - 1; orient >= 0; --orient)
	{
		for (std::size_t i = 0; i < myPlacements[orient].size(); ++i)
		{
			updateSingle((Orientation)orient, i);
		}

		updateResizeHandle((Orientation)orient);
	}
}

void BorderPanel::onResize()
{
	onUpdatePanel();
}

void BorderPanel::onAdd(std::shared_ptr<Widget> widget)
{
	place(widget, Center);
}

void BorderPanel::onRemove(std::shared_ptr<Widget> widget)
{
	removePlacementEntry(widget);
}

void BorderPanel::onNotify(Observable & subject, int message)
{
	Panel::onNotify(subject, message);

	if (message == NotifyAutoSized)
	{
		Widget * widgetPtr = dynamic_cast<Widget*>(&subject);

		if (widgetPtr)
		{
			auto result = retrieve(widgetPtr);
			if (result.first != OrientationCount && result.first != Center)
			{
				updateAutoSize(result.first, result.second);
				update(result.first, result.second);
			}
		}
	}
}

void BorderPanel::removePlacementEntry(std::shared_ptr<Widget> widget)
{
	auto result = retrieve(widget.get());
	if (result.first != OrientationCount)
	{
		if (result.first != Center)
		{
			mySideSettings[result.first].size -= myPlacements[result.first][result.second].size;
		}

		myPlacements[result.first].erase(myPlacements[result.first].begin() + result.second);
		update(result.first, result.second);
		widget->removeObserver(this);
	}
}

float BorderPanel::getSideOffset(Orientation side) const
{
	float offset = 0.f;
	for (std::size_t i = 0; i < myPlacements[side].size(); ++i)
	{
		offset += myPlacements[side][i].size;
	}
	return offset;
}

float BorderPanel::getEffectiveSideSize(Orientation side) const
{
	if (isResizable(side))
		return getSideSize(side) + 6.f;
	else
		return getSideSize(side);
}

std::pair<BorderPanel::Orientation, std::size_t> BorderPanel::retrieve(const Widget * widget) const
{
	for (int orient = 0; orient < OrientationCount; ++orient)
	{
		for (std::size_t i = 0; i < myPlacements[orient].size(); ++i)
		{
			if (myPlacements[orient][i].widget.lock().get() == widget)
				return std::make_pair((Orientation)orient, i);
		}
	}

	return std::make_pair(OrientationCount, 0);
}


}

