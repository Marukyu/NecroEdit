#include "Client/GUI2/Application.hpp"
#include "Shared/Utils/Utilities.hpp"

namespace gui2
{

Widget::Widget() :
	myPosition(0,0),
	mySize(0,0),
	myScale(1,1),
	myIsAutoManagingWidth(false),
	myIsAutoManagingHeight(false),
	myIsEnabled(true),
	myIsVisible(true),
	myZPosition(0),

	myIsMouseOver(0),
	myIsMouseDown(0),
	myIsClicked(0),

	myTransform(),
	myInverseTransform(),
	myIsTransformInvalid(true),
	myIsInverseTransformInvalid(true),

	myIsVertexCacheInvalid(true)
{
}

Widget::~Widget()
{
	while (!myLinkedWidgets.empty())
	{
		unlink(*myLinkedWidgets.begin());
	}
}

void Widget::setPosition(sf::Vector2f pos)
{
	if (myPosition != pos)
	{
		myPosition = pos;

		invalidateTransform();
		notify(NotifyMoved);
	}
}

void Widget::setPosition(float x, float y)
{
	if (myPosition.x != x || myPosition.y != y)
	{
		myPosition.x = x;
		myPosition.y = y;

		invalidateTransform();
		notify(NotifyMoved);
	}
}

sf::Vector2f Widget::getPosition() const
{
	return myPosition;
}

void Widget::setSize(sf::Vector2f size)
{
	if (mySize != size)
	{
		mySize = size;

		onResize();
		notify(NotifyResized);
	}
}

void Widget::setSize(float width, float height)
{
	if (mySize.x != width || mySize.y != height)
	{
		mySize.x = width;
		mySize.y = height;

		onResize();
		notify(NotifyResized);
	}
}

sf::Vector2f Widget::getSize() const
{
	return mySize;
}

void Widget::setRect(sf::FloatRect rect)
{
	setRect(rect.left, rect.top, rect.width, rect.height);
}

void Widget::setRect(float x, float y, float width, float height)
{
	setPosition(x, y);
	setSize(width, height);
}

sf::FloatRect Widget::getRect() const
{
	return sf::FloatRect(myPosition.x, myPosition.y, mySize.x, mySize.y);
}

void Widget::setAutoManagingSize(bool autoManage)
{
	setAutoManagingWidth(autoManage);
	setAutoManagingHeight(autoManage);
}

void Widget::setAutoManagingWidth(bool autoManage)
{
	myIsAutoManagingWidth = autoManage;
}

void Widget::setAutoManagingHeight(bool autoManage)
{
	myIsAutoManagingHeight = autoManage;
}

bool Widget::isAutoManagingWidth() const
{
	return myIsAutoManagingWidth;
}

bool Widget::isAutoManagingHeight() const
{
	return myIsAutoManagingHeight;
}

sf::Vector2f Widget::getAutomaticSize() const
{
	return myAutomaticSize;
}

void Widget::setScale(sf::Vector2f scale)
{
	if (myScale != scale)
	{
		myScale = scale;
		invalidateVertices();
	}
}

void Widget::setScale(float scaleX, float scaleY)
{
	setScale(sf::Vector2f(scaleX, scaleY));
}

sf::Vector2f Widget::getScale() const
{
	return myScale;
}

void Widget::setEnabled(bool enabled)
{
	if (myIsEnabled != enabled)
	{
		myIsEnabled = enabled;
		invalidateVertices();

		for (auto widget : myLinkedWidgets)
		{
			widget->invalidateVertices();
		}
	}
}

bool Widget::isEnabled() const
{
	return myIsEnabled && (!hasLinkParent() || getLinkParent()->isEnabled());
}

void Widget::setVisible(bool visible)
{
	if (myIsVisible != visible)
	{
		myIsVisible = visible;
		// send re-evaluation request for this widget to parent
		// container without clearing the vertex cache.
		invalidateRender();

		for (auto widget : myLinkedWidgets)
		{
			widget->invalidateRender();
		}
	}
}

bool Widget::isVisible() const
{
	return myIsVisible && (!hasLinkParent() || getLinkParent()->isVisible());
}


bool Widget::isFocused() const
{
	//return getParent() ? getParent()->getFocusedWidget() == shared_from_this() : false;
	return getParent() ? getParent()->isWidgetFocused(shared_from_this()) : false;
}

void Widget::acquireFocus()
{
	if (getParent())
		getParent()->setFocusedWidget(shared_from_this());
}


void Widget::setZPosition(int zpos)
{
	if (myZPosition != zpos)
	{
		myZPosition = zpos;
		if (getParent())
			getParent()->updateWidgetZPosition(shared_from_this());
	}
}

int Widget::getZPosition() const
{
	return myZPosition;
}

void Widget::sendToFront()
{
	if (getParent())
		getParent()->sendWidgetToFront(shared_from_this());
}

void Widget::sendToBack()
{
	if (getParent())
		getParent()->sendWidgetToBack(shared_from_this());
}

void Widget::link(std::shared_ptr<Widget> widget)
{
	if (!isLinked(widget))
	{
		myLinkedWidgets.insert(widget);
		widget->setLinkParent(shared_from_this());

		if (getParent())
		{
			if (widget->getParent() != getParent())
				getParent()->add(widget);
			else
				getParent()->updateWidgetZPosition(widget);
		}

		widget->invalidateTransform();
	}
}

void Widget::unlink(std::shared_ptr<Widget> widget)
{
	if (myLinkedWidgets.erase(widget))
	{
		widget->setLinkParent(nullptr);
		if (widget->getParent())
			widget->getParent()->remove(widget);
		widget->invalidateTransform();
	}
}

bool Widget::isLinked(std::shared_ptr<Widget> widget) const
{
	return myLinkedWidgets.count(widget) != 0;
}

const std::set<std::shared_ptr<Widget> > & Widget::getLinkedWidgets() const
{
	return myLinkedWidgets;
}

std::shared_ptr<Widget> Widget::getLinkParent() const
{
	return myLinkParent.lock();
}

bool Widget::hasLinkParent() const
{
	return !myLinkParent.expired();
}

bool Widget::checkLinkParentRecursive(std::shared_ptr<Widget> parent) const
{
	if (!parent)
		return true;

	if (!hasLinkParent())
		return false;

	if (getLinkParent() == parent)
		return true;

	return getLinkParent()->checkLinkParentRecursive(parent);
}

bool Widget::isMouseOver() const
{
	return myIsMouseOver;
}

bool Widget::isMouseDown(const Input & button) const
{
	return myIsMouseDown & convertMouseButtonBitmask(button);
}

bool Widget::isMousePressing(const Input & button) const
{
	return isMouseOver() && isMouseDown(button);
}

bool Widget::isClicked(const Input & button) const
{
	return myIsClicked & convertMouseButtonBitmask(button);
}


bool Widget::isVertexRenderable() const
{
	return true;
}

sf::Transform Widget::getTransform() const
{
	if (myIsTransformInvalid)
	{
		if (hasLinkParent())
			myTransform = getLinkParent()->getTransform();
		else
			myTransform = sf::Transform::Identity;

		myTransform.translate(getPosition());
		myTransform.scale(getScale());
		myIsTransformInvalid = false;
	}
	return myTransform;
}

sf::Transform Widget::getInverseTransform() const
{
	if (myIsInverseTransformInvalid)
	{
		myInverseTransform = getTransform().getInverse();
		myIsInverseTransformInvalid = false;
	}
	return myInverseTransform;
}

void Widget::invalidateVertices()
{
	myIsVertexCacheInvalid = true;

	invalidateRender();
}

void Widget::invalidateTransform()
{
	myIsTransformInvalid = true;
	myIsInverseTransformInvalid = true;

	for (auto widget : myLinkedWidgets)
		widget->invalidateTransform();

	invalidateRender();
}

void Widget::invalidateRender()
{
	if (getParent())
		getParent()->invalidateWidgetVertices(shared_from_this());
}

const std::vector<sf::Vertex> & Widget::getVertices() const
{
	return myVertexCache;
}

void Widget::process(const WidgetEvents & events)
{
	// check automatic size.
	if (isAutoManagingWidth() || isAutoManagingHeight())
	{
		sf::Vector2f autoSize = getAutomaticSize();
		setSize(isAutoManagingWidth() ? autoSize.x : getSize().x, isAutoManagingHeight() ? autoSize.y : getSize().y);
	}

	// clear click flags for all buttons.
	myIsClicked = 0;

	// set mouse-over flag. mouse position is already pre-transformed.
	bool mouseOver = events.mouseFocus && checkMouseover(events.mousePosition);

	if (myIsMouseOver != mouseOver)
	{
		myIsMouseOver = mouseOver;

		if (mouseOver)
		{
			onMouseOver();
		}
		else
		{
			onMouseAway();
		}
	}

	// analyze container events to find state changes.
	for (auto it = events.conEvents.mouseEvents.begin(); it != events.conEvents.mouseEvents.end(); ++it)
	{
		ContainerEvents::MouseEvent event = *it;
		MouseBitmask buttonMask = convertMouseButtonBitmask(event.button);

		sf::Vector2f convPos = events.transform.getInverse().transformPoint(event.position);

		// check if mouse is over widget (and widget is not obstructed).
		if (events.mouseFocus && checkMouseover(convPos))
		{
			// handle mouse presses/releases on the widget, check for successful click.
			if (event.isPress)
			{
				// bring widget to front on left mouse click.
				//if (event.button == sf::Mouse::Left)
				//	sendToFront();

				// add mouse button to bitmask.
				myIsMouseDown |= buttonMask;
				onMouseDown(convPos, event.button);
			}
			else if (myIsMouseDown & buttonMask)
			{
				myIsClicked |= buttonMask;
				onClick(convPos, event.button);
			}
		}

		// handle mouse button release.
		if (!event.isPress)
		{
			// remove mouse button from bitmask.
			if (myIsMouseDown & buttonMask)
			{
				myIsMouseDown &= ~buttonMask;
				onMouseUp(convPos, event.button);
			}
		}

	}

	// now process widget events.
	onProcess(events);
}

std::shared_ptr<Container> Widget::getParent() const
{
	/*if (hasLinkParent())
		return getLinkParent()->getParent();
	else
		return myParent.lock();
		*/
	return myParent.lock();
}

std::shared_ptr<Interface> Widget::getParentInterface() const
{
	return getParent() ? getParent()->getParentInterface() : onGetParentInterface();
}

Widget::Alignment Widget::convertAlignmentHorizontal(Alignment align)
{
	switch (align)
	{
	case AlignTopLeft:
	case AlignLeft:
	case AlignBottomLeft:
		return AlignLeft;

	case AlignTop:
	case AlignCenter:
	case AlignBottom:
		return AlignCenter;

	case AlignTopRight:
	case AlignRight:
	case AlignBottomRight:
		return AlignRight;

	default:
		return AlignCenter;
	}
}

Widget::Alignment Widget::convertAlignmentVertical(Alignment align)
{
	switch (align)
	{
	case AlignTopLeft:
	case AlignTop:
	case AlignTopRight:
		return AlignTop;

	case AlignLeft:
	case AlignCenter:
	case AlignRight:
		return AlignCenter;

	case AlignBottomLeft:
	case AlignBottom:
	case AlignBottomRight:
		return AlignBottom;

	default:
		return AlignCenter;
	}
}

Widget::Alignment Widget::combineAlignments(Alignment horiz, Alignment vert)
{
	switch (convertAlignmentHorizontal(horiz))
	{
	case AlignLeft:
		switch (convertAlignmentVertical(vert))
		{
		case AlignTop:	return AlignTopLeft;
		case AlignCenter: return AlignLeft;
		case AlignBottom: return AlignBottomLeft;
		default:		  return AlignCenter;
		}

	case AlignCenter:
	default:
		switch (convertAlignmentVertical(vert))
		{
		case AlignTop:	return AlignTop;
		case AlignCenter: return AlignCenter;
		case AlignBottom: return AlignBottom;
		default:		  return AlignCenter;
		}

	case AlignRight:
		switch (convertAlignmentVertical(vert))
		{
		case AlignTop:	return AlignTopRight;
		case AlignCenter: return AlignRight;
		case AlignBottom: return AlignBottomRight;
		default:		  return AlignCenter;
		}
	}
}


void Widget::setAutomaticSize(sf::Vector2f autoSize)
{
	if (myAutomaticSize != autoSize)
	{
		myAutomaticSize = autoSize;
		notify(NotifyAutoSized);
	}
}

sf::FloatRect Widget::getBaseRect() const
{
	sf::Vector2f size = getSize();
	return sf::FloatRect(0, 0, size.x, size.y);
}

Widget::BoxStyle::BoxStyle() :
	borderThickness(-2.f),
	hovered(false),
	pressed(false),
	focused(false),
	enabled(true),
	activated(false),
	color(100, 100, 100),
	//color(0, 0, 0),
	outlineColor(0, 0, 0)
	//outlineColor(255,255,255)
{

}

Widget::BoxStyle Widget::getDefaultBoxStyle() const
{
	BoxStyle style;
	style.hovered = isMouseOver() && isEnabled();
	style.pressed = isMousePressing() && isEnabled();
	//style.focused = isMouseDown() && !isMousePressing();
	style.focused = isFocused();
	style.enabled = isEnabled();
	style.activated = false;
	return style;
}

void Widget::clearVertices()
{
	myVertexCache.clear();
}

void Widget::vertexAdd(const std::vector<sf::Vertex> & vertices)
{
	myVertexCache.insert(myVertexCache.end(), vertices.begin(), vertices.end());

	sf::Vector2f whitePos = getParentInterface()->getParentApplication().getWhitePixel();

	// untexturize vertices.
	for (auto it = myVertexCache.end() - vertices.size(); it != myVertexCache.end(); ++it)
	{
		it->texCoords = whitePos;
	}
}

void Widget::vertexAddTextured(const std::vector<sf::Vertex> & vertices)
{
	myVertexCache.insert(myVertexCache.end(), vertices.begin(), vertices.end());
}

void Widget::vertexAddBox(BoxType type)
{
	vertexAddBox(getBaseRect(), type, getDefaultBoxStyle());
}

void Widget::vertexAddBox(BoxType type, const BoxStyle & style)
{
	vertexAddBox(getBaseRect(), type, style);
}

void Widget::vertexAddBox(sf::FloatRect rect, BoxType type)
{
	vertexAddBox(rect, type, getDefaultBoxStyle());
}

void Widget::vertexAddBox(sf::FloatRect rect, BoxType type, const BoxStyle & style)
{
	sf::Color baseColorA		(style.color),
	baseColorB		(style.color.r * 0.75, style.color.g * 0.75, style.color.b * 0.75, style.color.a),
	outlineColor	  (style.outlineColor),

	activeColorAdd	(  5, 60, 60),
	focusColorAdd	 (  5, 15, 30),
	mouseColorAdd	 ( 10, 30, 80),
	darkenColorMul	(127,127,127);

	/*
	// high contrast theme.
	sf::Color baseColorA		(style.color),
	baseColorB		(style.color),
	outlineColor	  (style.outlineColor),

	activeColorAdd	(255,255,255),
	focusColorAdd	 (0,0,0),
	mouseColorAdd	 (0,0,0),
	darkenColorMul	(255,255,255);
	*/

	float alpha = 1.f;

	if (style.pressed)
	{
		sf::Color tempSwap = baseColorB;
		baseColorB = baseColorA;
		baseColorA = tempSwap;
	}
	if (style.activated)
	{
		baseColorA += activeColorAdd;
		baseColorB += activeColorAdd;
	}
	if (type == BoxTypeBackground || type == BoxTypeDark)
	{
		baseColorA *= darkenColorMul;
		baseColorB *= darkenColorMul;
	}

	if (style.hovered)
	{
		baseColorA += mouseColorAdd;
		baseColorB += mouseColorAdd;
	}
	if (style.focused)
	{
		baseColorA += focusColorAdd;
		baseColorB += focusColorAdd;
	}

	if (!style.enabled)
		alpha *= 0.6f;

	if (type == BoxTypeBackground)
		alpha *= 0.75f;

	baseColorA.a *= alpha;
	baseColorB.a *= alpha;
	outlineColor.a *= alpha;

	// filling.
	vertexAddRectVGradient(rect, baseColorA, baseColorB);

	// outline.
	// top.
	vertexAddRect(sf::FloatRect(rect.left,
								-style.borderThickness+rect.top,
								rect.width,
								style.borderThickness), outlineColor);
	// right.
	vertexAddRect(sf::FloatRect(rect.width+rect.left,
								-style.borderThickness+rect.top,
								style.borderThickness,
								rect.height + style.borderThickness * 2.f), outlineColor);
	// bottom.
	vertexAddRect(sf::FloatRect(rect.left,
								rect.height+rect.top,
								rect.width,
								style.borderThickness), outlineColor);
	// left.
	vertexAddRect(sf::FloatRect(-style.borderThickness+rect.left,
								-style.borderThickness+rect.top,
								style.borderThickness,
								rect.height + style.borderThickness * 2.f), outlineColor);
}

void Widget::vertexAddRect(sf::FloatRect rect, sf::Color color)
{
	vertexAddRect(rect, color, color, color, color);
}

void Widget::vertexAddRectHGradient(sf::FloatRect rect, sf::Color colorLeft, sf::Color colorRight)
{
	vertexAddRect(rect, colorLeft, colorRight, colorRight, colorLeft);
}

void Widget::vertexAddRectVGradient(sf::FloatRect rect, sf::Color colorTop, sf::Color colorBottom)
{
	vertexAddRect(rect, colorTop, colorTop, colorBottom, colorBottom);
}

void Widget::vertexAddRect(sf::FloatRect rect, sf::Color colorTL, sf::Color colorTR, sf::Color colorBR, sf::Color colorBL)
{
	sf::Vector2f whitePos = getParentInterface() ?
							getParentInterface()->getParentApplication().getWhitePixel() :
							sf::Vector2f(0, 0);

	std::vector<sf::Vertex> vertices
	{
		sf::Vertex(sf::Vector2f(rect.left, rect.top), colorTL, whitePos),
		sf::Vertex(sf::Vector2f(rect.left+rect.width, rect.top), colorTR, whitePos),
		sf::Vertex(sf::Vector2f(rect.left, rect.top+rect.height), colorBL, whitePos),

		sf::Vertex(sf::Vector2f(rect.left+rect.width, rect.top), colorTR, whitePos),
		sf::Vertex(sf::Vector2f(rect.left+rect.width, rect.top+rect.height), colorBR, whitePos),
		sf::Vertex(sf::Vector2f(rect.left, rect.top+rect.height), colorBL, whitePos)
	};

	vertexAddTextured(vertices);
}

std::shared_ptr<const BitmapFont> Widget::getDefaultFont() const
{
	//return onGetFont(Application::FontDefault);
	return getDefaultFont(Application::FontDefault);
}

std::shared_ptr<const BitmapFont> Widget::getDefaultFont(int index) const
{
	//return onGetFont(index);
	return getParentInterface() ?
		   getParentInterface()->getParentApplication().getFont(index) :
		   nullptr;
}


void Widget::init()
{

}

void Widget::onProcess(const WidgetEvents & events)
{

}

void Widget::onDraw(sf::RenderTarget & target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	target.draw(myVertexCache.data(), myVertexCache.size(), sf::Triangles, states);
}

void Widget::onUpdateVertexCache()
{
	/*
	clearVertices();

	float xsize = 2.5f;

	std::vector<sf::Vertex> vertices(
	{
		sf::Vertex({0,0}, sf::Color::White),
		sf::Vertex({xsize,0}, sf::Color::White),
		sf::Vertex({0,xsize}, sf::Color::Red),

		sf::Vertex({xsize,0}, sf::Color::White),
		sf::Vertex({getSize().x, getSize().y - xsize}, sf::Color::Red),
		sf::Vertex({getSize().x - xsize, getSize().y}, sf::Color::Red),

		sf::Vertex({getSize().x - xsize, getSize().y}, sf::Color::Red),
		sf::Vertex({xsize,0}, sf::Color::White),
		sf::Vertex({0,xsize}, sf::Color::Red),

		sf::Vertex({getSize().x, getSize().y - xsize}, sf::Color::Red),
		sf::Vertex({getSize().x - xsize, getSize().y}, sf::Color::Red),
		sf::Vertex({getSize().x, getSize().y}, sf::Color::Red),
	});

	vertexAddBox(getBaseRect(), BoxTypeBackground);

	vertexAdd(vertices);

	// mirror.
	for (sf::Vertex & vertex : vertices)
		vertex.position.x = getSize().x - vertex.position.x;

	vertexAdd(vertices);

	btx::Text testText;
	testText.setFont(getDefaultFont());
	testText.setText("invalid");
	testText.setMaxSize(getSize());
	testText.setMaxSizeBehavior(btx::Text::Downscale);
	testText.setPosition(sf::Vector2f(sf::Vector2i(getSize() / 2.f - testText.getSize() / 2.f)));

	vertexAddTextured(testText.getVertices());
	*/
}

void Widget::onMouseOver()
{
	invalidateVertices();
}

void Widget::onMouseAway()
{
	invalidateVertices();
}

void Widget::onMouseDown(sf::Vector2f pos, Input button)
{
	invalidateVertices();
}

void Widget::onMouseUp(sf::Vector2f pos, Input button)
{
	invalidateVertices();
}

void Widget::onClick(sf::Vector2f pos, Input button)
{

}

void Widget::onFocus()
{
	invalidateVertices();
}

void Widget::onUnfocus()
{
	invalidateVertices();
}

void Widget::onResize()
{
	invalidateVertices();
}

void Widget::onParent(std::shared_ptr<Container> oldParent)
{

}

void Widget::onUpdateFonts()
{

}

bool Widget::checkMouseover(sf::Vector2f pos) const
{
	return getBaseRect().contains(pos);
}

/*
std::shared_ptr<const btx::Font> Widget::onGetFont(int index) const
{
	return getParent()->getFont(index);
}
*/

std::shared_ptr<Interface> Widget::onGetParentInterface() const
{
	return nullptr;
}

void Widget::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	onDraw(target, states);
}

void Widget::setParent(std::shared_ptr<Container> parent)
{
	myParent = parent;
	notify(NotifyReparented);
}

void Widget::setLinkParent(std::shared_ptr<Widget> linkParent)
{
	myLinkParent = linkParent;
}

void Widget::updateVertexCache()
{
	if (myIsVertexCacheInvalid)
	{
		onUpdateVertexCache();
		myIsVertexCacheInvalid = false;
	}
}

Widget::MouseBitmask Widget::convertMouseButtonBitmask(Input button)
{
	if (button.getType() != Input::Mouse)
		return 0;

	switch (button.getCode())
	{
		case sf::Mouse::Left:	   return 1 << 0;
		case sf::Mouse::Right:	  return 1 << 1;
		case sf::Mouse::Middle:	 return 1 << 2;
		case sf::Mouse::XButton1:   return 1 << 3;
		case sf::Mouse::XButton2:   return 1 << 4;
		case Input::WheelUp:	 return 1 << 5;
		case Input::WheelDown:   return 1 << 6;

		default:					return 0;
	}
}

} // namespace gui2
