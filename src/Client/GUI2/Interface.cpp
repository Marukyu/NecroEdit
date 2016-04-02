#include "Client/GUI2/Application.hpp"
#include "Shared/Utils/MiscMath.hpp"

#include <limits>

namespace gui2
{

Interface::Interface() :
	myIsFullscreen(false),
	myIsMouseCursorVisible(true),
	myHasFocus(true),
	myRootContainer(std::make_shared<RootContainer>()),
	myParentApplication(0),
	myWindowSize(1280, 720),
	myWindowTitle("Unnamed window")
{
}

Interface::~Interface()
{
	
}

Container & Interface::getRootContainer()
{
	return *myRootContainer;
}

const Container & Interface::getRootContainer() const
{
	return *myRootContainer;
}

Application & Interface::getParentApplication() const
{
	return *myParentApplication;
}

void Interface::setTitle(sf::String title)
{
	myWindowTitle = title;
	myWindow.setTitle(myWindowTitle);
}

sf::String Interface::getTitle() const
{
	return myWindowTitle;
}

void Interface::resize(sf::Vector2u size, bool fullscreen)
{
	// if window is or will be fullscreen, recreate it.
	if (myIsFullscreen || fullscreen)
	{
		myWindow.close();
		myHasFocus = true;
		myWindowSize = size;
		myIsFullscreen = fullscreen;
		openWindow();
	}
	else
	{
		myWindow.setSize(size);
	}

	myWindowSize = myWindow.getSize();
}

sf::Vector2u Interface::getSize() const
{
	return myWindowSize;
}

bool Interface::isFullscreen() const
{
	return myIsFullscreen;
}

bool Interface::isFocused() const
{
	return myHasFocus;
}

void Interface::setMouseCursorVisible(bool visible)
{
	if (myIsMouseCursorVisible != visible)
	{
		myIsMouseCursorVisible = visible;
		myWindow.setMouseCursorVisible(myIsMouseCursorVisible);
	}
}

bool Interface::isMouseCursorVisible() const
{
	return myIsMouseCursorVisible;
}


sf::RenderTarget & Interface::getRenderTarget()
{
	return myWindow;
}

const sf::RenderTarget & Interface::getRenderTarget() const
{
	return myWindow;
}

void Interface::openWindow()
{
	sf::ContextSettings context;
	//context.antialiasingLevel = 4;

	if (myIsFullscreen)
		myWindow.create(findGoodVideoMode(myWindowSize), myWindowTitle, sf::Style::Fullscreen, context);
	else
		myWindow.create(sf::VideoMode(myWindowSize.x, myWindowSize.y), myWindowTitle, sf::Style::Default, context);

	myWindow.setMouseCursorVisible(myIsMouseCursorVisible);
}

void Interface::closeWindow()
{
	myWindow.close();
}

bool Interface::isWindowOpen() const
{
	return myWindow.isOpen();
}


void Interface::init()
{

}

void Interface::process()
{
	myContainerEvents.reset();

	for (sf::Event event; myWindow.pollEvent(event); )
	{
		switch (event.type)
		{
		case sf::Event::Closed:
			myWindow.close();
			break;

		case sf::Event::GainedFocus:
			myHasFocus = true;
			break;

		case sf::Event::LostFocus:
			myHasFocus = false;
			break;

		case sf::Event::Resized:
			myWindowSize.x = event.size.width;
			myWindowSize.y = event.size.height;
			break;

		default:
			break;
		}

		// auto-process container events.
		myContainerEvents.processEvent(event);

		// allow derived interface to process further events.
		onEvent(event);
	}

	myRootContainer->setSize(sf::Vector2f(getSize()));

	WidgetEvents wgtEvents = WidgetEvents(myContainerEvents, sf::Transform::Identity, isFocused(), isFocused());

	myRootContainer->process(wgtEvents);
	onProcess(wgtEvents);

	myRootContainer->updateVertexCache();

	myWindow.setView(sf::View(sf::FloatRect(0, 0, getSize().x, getSize().y)));
	myWindow.clear(sf::Color(25, 45, 75));

	sf::RenderStates states;
	states.texture = getParentApplication().getMainTexture();
	myRootContainer->draw(myWindow, states);

	onRender();
	myWindow.display();
}

void Interface::onEvent(const sf::Event & event)
{

}

void Interface::onProcess(const WidgetEvents & events)
{

}

void Interface::onRender()
{

}

bool Interface::RootContainer::isClippingWidgets() const
{
	return false;
}

void Interface::RootContainer::onProcessContainer(const WidgetEvents & events)
{
	if (getParentInterface())
		getParentInterface()->onProcess(events);
}

void Interface::internalInit(Application * parentApp)
{
	myParentApplication = parentApp;
	myRootContainer->setParentInterface(shared_from_this());
	openWindow();
}

sf::VideoMode Interface::findGoodVideoMode(sf::Vector2u compare) const
{
	const std::vector<sf::VideoMode> modes = sf::VideoMode::getFullscreenModes();
	if (modes.empty())
		return sf::VideoMode::getDesktopMode();

	sf::VideoMode bestMode = sf::VideoMode::getDesktopMode();

	// mode "quality", difference between attempted and available mode. the lower the better.
	int bestModeQuality = std::numeric_limits<int>::max();

	for (unsigned int i=0; i<modes.size(); ++i)
	{
		const int curModeQuality =
			iAbs<int>(modes[i].width - compare.x) + iAbs<int>(modes[i].height - compare.y) -
			modes[i].bitsPerPixel;

		if (curModeQuality < bestModeQuality)
		{
			bestMode = modes[i];
			bestModeQuality = curModeQuality;
		}
	}

	return bestMode;
}

} // namespace gui2
