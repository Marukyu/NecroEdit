#include "Client/GUI2/Widgets/Window.hpp"
#include "Client/GUI2/Application.hpp"
#include "Client/GUI2/Generator.hpp"
#include "Shared/Utils/StrNumCon.hpp"
#include "Shared/Utils/VectorMul.hpp"

namespace gui2
{

const float Window::cvTitleBarHeight = 20,
			Window::cvBorderThickness = 2,
			Window::cvResizeGrab = Window::cvBorderThickness * 8;
const int   Window::cvResizeAnimationMax = 30;

Ptr<Window> Window::make()
{
	Ptr<Window> widget = std::make_shared<Window>();
	widget->init();
	return widget;
}

void Window::open()
{
	setVisible(true);
}
void Window::openAutoRemove()
{
	myAutoRemove = true;
	open();
}
void Window::close()
{
	setVisible(false);

	if (myAutoRemove && getParent())
	{
		getParent()->remove(shared_from_this());
	}
}
bool Window::isOpen() const
{
	return isVisible();
}

void Window::setCaption(std::string text)
{
	myCaption.setText(text);
}

std::string Window::getCaption() const
{
	return myCaption.getText();
}

void Window::setMaximized(bool maximized)
{
	if (isMaximized() == maximized) return;

	myIsMaximized = maximized;

	myButtonMax->setText(isMaximized() ? "7" : "6");

	if (maximized)
	{
		myOriginalRect = getRect();
	}
	else
	{
		setRect(myOriginalRect);
	}
}
bool Window::isMaximized() const
{
	return myIsMaximized;
}

void Window::setEnforcedRatio(float ratio)
{
	myEnforcedRatio = ratio;
}
float Window::getEnforcedRatio() const
{
	return myEnforcedRatio;
}
bool Window::isEnforcingRatio() const
{
	return myEnforcedRatio > 0;
}
void Window::setMinSize(sf::Vector2f minSize)
{
	myMinSize = minSize;
}
sf::Vector2f Window::getMinSize() const
{
	return myMinSize;
}
void Window::setMaxSize(sf::Vector2f maxSize)
{
	myMaxSize = maxSize;
}
sf::Vector2f Window::getMaxSize() const
{
	return myMaxSize;
}
void Window::setFlags(unsigned int flags)
{
	//if (myFlags != flags)
	{
		myFlags = flags;
		myButtonClose->setText((myFlags & FlagOkay) ? "4" : "5");
	}
}
void Window::addFlags(unsigned int flags)
{
	setFlags(myFlags | flags);
}
void Window::removeFlags(unsigned int flags)
{
	setFlags(myFlags & (~flags));
}
bool Window::hasFlag(unsigned int flag) const
{
	return myFlags & flag;
}
unsigned int Window::getFlags() const
{
	return myFlags;
}

void Window::init()
{
	Container::init();

	myButtonClose = Button::make();
	myButtonMax = Button::make();

	myAutoRemove = false;

	setEnforcedRatio(-1);
	setFlags(DefaultFlags);
	myIsMaximized = false;
	setMinSize(sf::Vector2f(cvTitleBarHeight * 4.f, cvTitleBarHeight));
	setMaxSize(sf::Vector2f(-1, -1));

	/*
	myResizeHandle.setPointCount(3);
	myResizeHandle.setFillColor(sf::Color(150,200,250));
	myResizeHandle.setOutlineColor(sf::Color(75,100,125));
	myResizeHandle.setOutlineThickness(cvBorderThickness);
	*/
	myResizeHandleAnimation = 0;
	myTitleBarDownCache = true;

	link(myButtonClose);
	myButtonClose->setTextFont(Application::FontSymbolSmall);
	myButtonClose->setText("5");
	myButtonClose->setPosition(0,0);
	myButtonClose->setSize(cvTitleBarHeight + cvBorderThickness*2.f, cvTitleBarHeight + cvBorderThickness*2.f);

	link(myButtonMax);
	myButtonMax->setTextFont(Application::FontSymbolSmall);
	myButtonMax->setText("6");
	myButtonMax->setPosition(0,0);
	myButtonMax->setSize(cvTitleBarHeight + cvBorderThickness*2.f, cvTitleBarHeight + cvBorderThickness*2.f);

	setZPosition(10);
}

bool Window::isPosOverTitleBar(sf::Vector2f pos) const
{
	return getTitleBarRect().contains(pos);
}

bool Window::isTitleBarDown() const
{
	/*
	if (getParent())
		return getPosition().y <= cvBorderThickness * 2 - getTransformInfo().offset.y;
	else */
		return getPosition().y <= cvBorderThickness * 2;
}

sf::FloatRect Window::getTitleBarRect() const
{
	return sf::FloatRect(-cvBorderThickness,
		-cvBorderThickness + (isTitleBarDown() ? getSize().y + cvBorderThickness : -cvTitleBarHeight - cvBorderThickness),
		getSize().x + 2.f * cvBorderThickness - getTitleBarButtonWidth(),
		cvTitleBarHeight + 2.f * cvBorderThickness);
}

float Window::getTitleBarButtonWidth() const
{
	float width = 0;
	if (myButtonClose->isVisible())
		width += myButtonClose->getSize().x - cvBorderThickness;
	if (myButtonMax->isVisible())
		width += myButtonMax->getSize().x - cvBorderThickness;
	return width;
}

sf::FloatRect Window::getTRCornerRect() const
{
	sf::FloatRect ret(getRect().width - cvResizeGrab + cvBorderThickness - (!isTitleBarDown() ? getTitleBarButtonWidth() : 0),
					  -cvBorderThickness,
					  cvResizeGrab,
					  cvResizeGrab);
	if (!isTitleBarDown())
		ret.top -= cvTitleBarHeight;
	return ret;
}
sf::FloatRect Window::getTLCornerRect() const
{
	sf::FloatRect ret(-cvBorderThickness,
					  -cvBorderThickness,
					  cvResizeGrab,
					  cvResizeGrab);
	if (!isTitleBarDown())
		ret.top -= cvTitleBarHeight;
	return ret;
}
sf::FloatRect Window::getBLCornerRect() const
{
	sf::FloatRect ret(-cvBorderThickness,
					  getRect().height - cvResizeGrab + cvBorderThickness,
					  cvResizeGrab,
					  cvResizeGrab);
	if (isTitleBarDown())
		ret.top += cvTitleBarHeight + cvBorderThickness;
	return ret;
}
sf::FloatRect Window::getBRCornerRect() const
{
	sf::FloatRect ret(getRect().width - cvResizeGrab + cvBorderThickness - (isTitleBarDown() ? getTitleBarButtonWidth() : 0),
					  getRect().height - cvResizeGrab + cvBorderThickness,
					  cvResizeGrab,
					  cvResizeGrab);
	if (isTitleBarDown())
		ret.top += cvTitleBarHeight + cvBorderThickness;
	return ret;
}

void Window::onResize()
{
	invalidateAllWidgetVertices();
	notify(NotifyContainerBox);
	onResizeWindow();
}

void Window::onProcessContainer(WidgetEvents & events)
{
	// load fonts for caption and size readout.
	if (!myCaption.hasFont())
		myCaption.setFont(getDefaultFont(Application::FontDefault));
	if (!mySizeReadout.hasFont())
		mySizeReadout.setFont(getDefaultFont(Application::FontDefault));

	// misc.
	bool freeDrag = hasFlag(FlagMove) && events.isShiftPressed() && events.isControlPressed();
	sf::Vector2f newSize(getSize());
	sf::Vector2f parSize;
	float tbHeight = hasFlag(FlagMinimal) ? 0 : getTitleBarRect().height - cvBorderThickness;

	// get size of parent container.
	if (getParent())
	{
		sf::FloatRect parBox(getParent()->getContainerBoundingBox());
		parSize = sf::Vector2f(parBox.width, parBox.height);

		if (myParSizeCache != parSize)
		{
			myParSizeCache = parSize;
			invalidateVertices();
		}
	}

	// move/resize.
	if (isMouseDown() && !isMaximized())
	{
		// start dragging.
		if (myDragMode == DragNone)
		{
			if (hasFlag(FlagResize) && getTRCornerRect().contains(events.mousePosition))
			{
				myDragMode = ResizeTR;
			}
			else if (hasFlag(FlagResize) && getTLCornerRect().contains(events.mousePosition))
			{
				myDragMode = ResizeTL;
			}
			else if (hasFlag(FlagResize) && getBLCornerRect().contains(events.mousePosition))
			{
				myDragMode = ResizeBL;
			}
			else if (hasFlag(FlagResize) && getBRCornerRect().contains(events.mousePosition))
			{
				myDragMode = ResizeBR;
			}
			else if (hasFlag(FlagMove) && (isPosOverTitleBar(events.mousePosition) || freeDrag))
			{
				myDragMode = Dragging;
			}
			else
				myDragMode = DragFail;

			if ((myDragMode == ResizeTR || myDragMode == ResizeTL ||
				 myDragMode == ResizeBL || myDragMode == ResizeBR))
			{
				myDragOffset = convertDragPosition(events.mousePosition) - getSize();
				myResizeOrigin = getPosition() + getDragResizeTranslation();

			}
			else if (myDragMode == Dragging)
			{
				myDragOffset = convertDragPosition(events.mousePosition);
				myResizeOrigin = getPosition();
			}
		}

		if ((myDragMode == ResizeTR || myDragMode == ResizeTL ||
			 myDragMode == ResizeBL || myDragMode == ResizeBR) && hasFlag(FlagResize))
		{
			// process resizing.
			newSize = convertDragPosition(events.mousePosition) - myDragOffset;
			events.mouseFocus = false;
		}
		else if (myDragMode == Dragging  && hasFlag(FlagMove))
		{
			// process moving.
			setPosition(events.mousePosition + getPosition() - myDragOffset);
			events.mouseFocus = false;
		}
	}
	else
	{
		myDragMode = DragNone;
	}

	// process maximized window.
	if (getParent() && isMaximized())
	{
		if (isEnforcingRatio())
		{
			// first try horizontal fill. if it exceeds screen height, use vertical fill.
			if (parSize.x * getEnforcedRatio() <= parSize.y - tbHeight)
				newSize.x = parSize.x;
			else
				newSize.x = (parSize.y - tbHeight) / getEnforcedRatio();

			// apply ratio.
			newSize.y = newSize.x * getEnforcedRatio();

			// center window.
			//corRect.left = (parSize.x - corRect.width) / 2.f;
			//corRect.top = (parSize.y - corRect.height + tbHeight) / 2.f;
		}
		else
		{
			//corRect = sf::FloatRect(0,tbHeight,parSize.x,parSize.y-tbHeight);
			newSize = parSize;
			newSize.y -= tbHeight;
		}
	}

	// enforce size limits. only x direction is respected for enforced-ratio windows.
	newSize.x = std::max(getMinSize().x, newSize.x);
	newSize.y = std::max(getMinSize().y, newSize.y);
	if (getMaxSize().x > 0) newSize.x = std::min(getMaxSize().x, newSize.x);
	if (getMaxSize().y > 0) newSize.y = std::min(getMaxSize().y, newSize.y);

	// enforce position limits.
	if (getParent())
	{
		if (getPosition().x < -getSize().x + cvBorderThickness + getTitleBarButtonWidth())
			setPosition(-getSize().x + cvBorderThickness + getTitleBarButtonWidth(), getPosition().y);
		if (getPosition().x > parSize.x - cvBorderThickness)
			setPosition(parSize.x - cvBorderThickness, getPosition().y);
		if (getPosition().y < -getSize().y)
			setPosition(getPosition().x, -getSize().y);
		if (getPosition().y > parSize.y)
			setPosition(getPosition().x, parSize.y);
	}

	// apply new size.
	if (isMaximized())
	{
		// apply size.
		setSize(newSize);

		// center window, pay attention to title bar.
		setPosition((parSize.x - newSize.x) / 2.f, (parSize.y - newSize.y + tbHeight) / 2.f);
	}
	else if (newSize != getSize())
	{
		// enforce ratio.
		if (isEnforcingRatio())
			newSize.y = newSize.x * getEnforcedRatio();

		// apply size and resize-mode dependent position offset.
		setSize(newSize);
		setPosition(myResizeOrigin - getDragResizeTranslation());
	}

	// check if title bar changed position.
	bool titleBarMoved = (myTitleBarDownCache != isTitleBarDown());
	myTitleBarDownCache = isTitleBarDown();

	// place title bar buttons.
	float titleButtonPos = 0;

	if (hasFlag(FlagClose))
	{
		titleButtonPos += cvTitleBarHeight + cvBorderThickness;
		myButtonClose->setPosition(sf::Vector2f(getSize().x - titleButtonPos, getTitleBarRect().top));
	}
	myButtonClose->setVisible(!hasFlag(FlagMinimal) && hasFlag(FlagClose));

	if (hasFlag(FlagMaximize))
	{
		titleButtonPos += cvTitleBarHeight + cvBorderThickness;
		myButtonMax->setPosition(sf::Vector2f(getSize().x - titleButtonPos, getTitleBarRect().top));
	}
	myButtonMax->setVisible(!hasFlag(FlagMinimal) && hasFlag(FlagMaximize));

	if (myButtonClose->isClicked() && !freeDrag && hasFlag(FlagClose) && onClose())
	{
		close();
	}
	if (myButtonMax->isClicked() && !freeDrag && hasFlag(FlagMaximize))
	{
		setMaximized(!isMaximized());
	}


	// show resize handle.
	bool displayResizeHandle = false;

	if (((isMouseOver() && events.mouseFocus) || myDragMode == ResizeTR || myDragMode == ResizeTL ||
		myDragMode == ResizeBL || myDragMode == ResizeBR) && !isMaximized() && hasFlag(FlagResize))
	{
		myResizeHandleAnimation++;
		if (myResizeHandleAnimation >= cvResizeAnimationMax)
			myResizeHandleAnimation = -cvResizeAnimationMax;

		sf::Vector2f resizeHandlePos, resizeHandleDir;

		if (myDragMode == ResizeTR || getTRCornerRect().contains(events.mousePosition)) {
			displayResizeHandle = true;
			resizeHandleDir = sf::Vector2f(-1, 1);
			resizeHandlePos = sf::Vector2f(getSize().x - (!isTitleBarDown() ? getTitleBarButtonWidth() + cvBorderThickness : 0), isTitleBarDown() ? 0 : -cvTitleBarHeight-cvBorderThickness);

		} else
		if (myDragMode == ResizeTL || getTLCornerRect().contains(events.mousePosition)) {
			displayResizeHandle = true;
			resizeHandleDir = sf::Vector2f(1, 1);
			resizeHandlePos = sf::Vector2f(0, isTitleBarDown() ? 0 : -cvTitleBarHeight-cvBorderThickness);
		} else
		if (myDragMode == ResizeBL || getBLCornerRect().contains(events.mousePosition)) {
			displayResizeHandle = true;
			resizeHandleDir = sf::Vector2f(1, -1);
			resizeHandlePos = sf::Vector2f(0, getSize().y + (isTitleBarDown() ? cvTitleBarHeight+cvBorderThickness : 0));
		} else
		if (myDragMode == ResizeBR || getBRCornerRect().contains(events.mousePosition)) {
			displayResizeHandle = true;
			resizeHandleDir = sf::Vector2f(-1, -1);
			resizeHandlePos = sf::Vector2f(getSize().x - (isTitleBarDown() ? getTitleBarButtonWidth() + cvBorderThickness : 0), getSize().y + (isTitleBarDown() ? cvTitleBarHeight+cvBorderThickness : 0));
		}

		if (displayResizeHandle)
		{
			static sf::Color resizeColor1(150,200,250);
			static sf::Color resizeColor2(75,100,125);
			float handleAnim = cvResizeGrab - (5.f * std::abs(myResizeHandleAnimation) / (float)cvResizeAnimationMax);
			resizeHandlePos -= cvBorderThickness * resizeHandleDir;
			myResizeHandleVertices.clear();
			myResizeHandleVertices.push_back(sf::Vertex(resizeHandlePos, resizeColor2));
			myResizeHandleVertices.push_back(sf::Vertex(resizeHandlePos + sf::Vector2f(resizeHandleDir.x * handleAnim, 0), resizeColor2));
			myResizeHandleVertices.push_back(sf::Vertex(resizeHandlePos + sf::Vector2f(0, resizeHandleDir.y * handleAnim), resizeColor2));
			myResizeHandleVertices.push_back(sf::Vertex(resizeHandlePos + resizeHandleDir * cvBorderThickness, resizeColor1));
			myResizeHandleVertices.push_back(sf::Vertex(resizeHandlePos + resizeHandleDir * cvBorderThickness + sf::Vector2f(resizeHandleDir.x * (handleAnim - cvBorderThickness * 3.414f), 0), resizeColor1));
			myResizeHandleVertices.push_back(sf::Vertex(resizeHandlePos + resizeHandleDir * cvBorderThickness + sf::Vector2f(0, resizeHandleDir.y * (handleAnim - cvBorderThickness * 3.414f)), resizeColor1));
			invalidateVertices();
		}
	}

	// invalidate vertices to clear resize handles.
	if (!displayResizeHandle && !myResizeHandleVertices.empty())
	{
		myResizeHandleVertices.clear();
		invalidateVertices();
	}

	// display size readout.
	if (myDragMode == ResizeTR || myDragMode == ResizeTL ||
		myDragMode == ResizeBL || myDragMode == ResizeBR)
	{
		mySizeReadout.setText(cNtoS((int)getSize().x) + "x" + cNtoS((int)getSize().y));
		mySizeReadout.setPosition(sf::Vector2f(sf::Vector2i(getSize() - mySizeReadout.getSize()) / 2));
	}
	else
	{
		// clear size readout.
		mySizeReadout.setText("");
	}

	// redraw if title bar changed location.
	if (titleBarMoved)
	{
		invalidateVertices();
	}

	/*
	// display: window decorations.

	if (myFlags & FlagMinimal)
	{
		myBackgroundBox = generateBox(getBaseRect(), BoxBackground, false, 0.75f, cvBorderThickness);
	}
	else
	{
		myTitleBarBox = generateBox(getTitleBarRect(), (myDragMode == Dragging /..* && hasFlag(FlagResize) *../) ? BoxDown : BoxBase,
									(myDragMode == Dragging || (isPosOverTitleBar(convertPosition(events.mousePosition))
									&& isMouseOver() && mouseFocus && myDragMode != DragFail)) && !isMaximized() && hasFlag(FlagMove),
									1.f, cvBorderThickness);


		myBackgroundBox = generateBox(getBaseRect(), BoxBackground, false, isFocused() ? 1.f : 0.75f, cvBorderThickness);

		myCaption.setMaxSize(sf::Vector2f(getTitleBarRect().width, cvTitleBarHeight));
		myCaption.setMaxSizeBehavior(btx::Text::Downscale);
		myCaption.setPosition((int)(getTitleBarRect().left + (getTitleBarRect().width - myCaption.getSize().x) / 2),
							  (int)(getTitleBarRect().top  + (getTitleBarRect().height - myCaption.getSize().y) / 2));
	}


	// display: size readout.
	if (myDisplaySizeReadout)
	{
		mySizeReadout.setText(cNtoS((int)getSize().x) + "x" + cNtoS((int)getSize().y));
		mySizeReadout.setPosition(sf::Vector2f(sf::Vector2i(getSize() - mySizeReadout.getSize()) / 2));
		mySizeReadoutBox = generateBox(sf::FloatRect(mySizeReadout.getPosition().x-5,
													 mySizeReadout.getPosition().y-3,
													 mySizeReadout.getSize().x+10,
													 mySizeReadout.getSize().y+6), BoxDark, false, 0.5f);
	}
	*/

	// processing hook.
	onProcessWindow(events);
}

void Window::onUpdateVertexCachePrepend()
{
	BoxStyle backgroundStyle;
	backgroundStyle.borderThickness = cvBorderThickness;

	vertexAddBox(getBaseRect(), BoxTypeBackground, backgroundStyle);
}

void Window::onUpdateVertexCacheAppend()
{
	static sf::Color darkA(0,15,30,255);
	static sf::Color darkB(0,10,20,255);
	//static sf::Color darkA(255, 0, 0,255);
	//static sf::Color darkB(0, 255, 0,255);

	// darken areas outside of window if in maximized mode without decorations.
	if (getParent() && isMaximized() && isEnforcingRatio() /* && hasFlag(FlagMinimal) */)
	{
		sf::FloatRect parBox(getParent()->getContainerBoundingBox());
		sf::Vector2f parSize = sf::Vector2f(parBox.width, parBox.height);

		if (getPosition().y > 0.f)
		{
			vertexAddRectVGradient(sf::FloatRect(-getPosition().x, -getPosition().y, parSize.x, getPosition().y - cvBorderThickness),
								   darkA, darkB);
		}

		if (getPosition().y + getSize().y < parSize.y)
		{
			vertexAddRectVGradient(sf::FloatRect(-getPosition().x, getSize().y + cvBorderThickness, parSize.x, getPosition().y - cvBorderThickness),
								   darkB, darkA);
		}

		if (getPosition().x > 0.f)
		{
			vertexAddRectHGradient(sf::FloatRect(-getPosition().x, -getPosition().y, getPosition().x - cvBorderThickness, parSize.y),
								   darkA, darkB);
		}

		if (getPosition().x + getSize().x < parSize.x)
		{
			vertexAddRectHGradient(sf::FloatRect(getSize().x + cvBorderThickness, -getPosition().y, getPosition().x - cvBorderThickness, parSize.y),
								   darkB, darkA);
		}
	}

	// render titlebar.
	if (!hasFlag(FlagMinimal))
	{
		BoxStyle titlebarStyle;
		titlebarStyle.borderThickness = -cvBorderThickness;
		titlebarStyle.focused = isFocused();

		vertexAddBox(getTitleBarRect(), BoxTypeBasic, titlebarStyle);

		myCaption.setMaxSize(sf::Vector2f(getTitleBarRect().width - cvBorderThickness * 3.f, getTitleBarRect().height));
		myCaption.setMaxSizeBehavior(btx::BitmapText::Downscale);
		myCaption.setPosition((int)(getTitleBarRect().left + (getTitleBarRect().width - myCaption.getSize().x) / 2),
							  (int)(getTitleBarRect().top  + (getTitleBarRect().height - myCaption.getSize().y) / 2));
		myCaption.updateMinor();

		vertexAddTextured(myCaption.getVertices());
	}

	// render resize handles.
	vertexAdd(myResizeHandleVertices);

	// render size display.
	if (!mySizeReadout.getText().empty())
	{
		BoxStyle sizeReadoutStyle;
		sizeReadoutStyle.borderThickness = -cvBorderThickness;

		vertexAddBox(sf::FloatRect(mySizeReadout.getPosition().x-5,
								   mySizeReadout.getPosition().y-3,
								   mySizeReadout.getSize().x+10,
								   mySizeReadout.getSize().y+6), BoxTypeDark, sizeReadoutStyle);

		vertexAddTextured(mySizeReadout.getVertices());
	}
}

bool Window::checkMouseover(sf::Vector2f pos) const
{
	if (isMaximized())
		return true;

	sf::FloatRect rect = getBaseRect();

	rect.left   -= cvBorderThickness;
	rect.top	-= cvBorderThickness;
	rect.width  += cvBorderThickness * 2;
	rect.height += cvBorderThickness * 2;
	if (!(myFlags & FlagMinimal))
	{
		rect.top	-= cvTitleBarHeight;
		rect.height += cvTitleBarHeight;

		if (isTitleBarDown())
			rect.top += cvBorderThickness + cvTitleBarHeight;
	}

	return rect.contains(pos);
}

void Window::onProcessWindow(const WidgetEvents & events)
{
	// nothing.
}

void Window::onResizeWindow()
{

}

bool Window::onClose()
{
	// always close by default.
	return true;
}

sf::Vector2f Window::convertDragPosition(sf::Vector2f pos) const
{
	switch (myDragMode)
	{
	case ResizeTR:
		return sf::Vector2f(pos.x, getSize().y - pos.y);

	case ResizeTL:
		return getSize() - pos;

	case ResizeBL:
		return sf::Vector2f(getSize().x - pos.x, pos.y);

	case ResizeBR:
	default:
		return pos;
	}
}

sf::Vector2f Window::getDragResizeTranslation() const
{
	switch (myDragMode)
	{
	case ResizeTR: return sf::Vector2f(0,getSize().y);
	case ResizeTL: return getSize();
	case ResizeBL: return sf::Vector2f(getSize().x,0);
	case ResizeBR: return sf::Vector2f();
	default: return sf::Vector2f();
	}
}


} // namespace gui2
