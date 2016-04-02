#include "Client/GUI2/Internal/WidgetEvents.hpp"

namespace gui2
{

void ContainerEvents::reset()
{
	heldInputs.remove(Input::WheelUp);
	heldInputs.remove(Input::WheelDown);
	textInput.clear();
	mouseWheelDelta = 0;
	mouseEvents.clear();
	keyEvents.clear();
	pressedInputs.clear();
}

void ContainerEvents::processEvent(const sf::Event & event)
{
	switch (event.type)
	{
	case sf::Event::MouseMoved:
		mousePosition.x = event.mouseMove.x;
		mousePosition.y = event.mouseMove.y;
		break;

	case sf::Event::MouseButtonPressed:
		mouseEvents.push_back(ContainerEvents::MouseEvent(true, sf::Vector2f(event.mouseButton.x, event.mouseButton.y), Input(event) ));
		pressedInputs.add(Input(event));
		heldInputs.add(Input(event));
		break;

	case sf::Event::MouseButtonReleased:
		mouseEvents.push_back(ContainerEvents::MouseEvent(false, sf::Vector2f(event.mouseButton.x, event.mouseButton.y), Input(event) ));
		heldInputs.remove(Input(event));
		break;

	case sf::Event::KeyPressed:
	{
		keyEvents.push_back(ContainerEvents::KeyEvent(true, Input(event) ));
		Input key(event);
		if (key != Input())
		{
			pressedInputs.add(key);
			heldInputs.add(key);
		}
		break;
	}

	case sf::Event::KeyReleased:
		keyEvents.push_back(ContainerEvents::KeyEvent(false, Input(event) ));
		heldInputs.remove(Input(event));
		break;

	case sf::Event::TextEntered:
		if (event.text.unicode < 256)
			textInput += (unsigned char)event.text.unicode;
		break;

	case sf::Event::LostFocus:
		heldInputs.clear();
		break;

	case sf::Event::MouseWheelMoved:
		mouseEvents.push_back(ContainerEvents::MouseEvent(true, sf::Vector2f(event.mouseWheel.x, event.mouseWheel.y), Input(event) ));
		mouseEvents.push_back(ContainerEvents::MouseEvent(false, sf::Vector2f(event.mouseWheel.x, event.mouseWheel.y), Input(event) ));
		mouseWheelDelta = event.mouseWheel.delta;
		break;

	default:
		break;
	}
}

bool WidgetEvents::isShiftPressed() const
{
	return heldInputs.contains(sf::Keyboard::LShift) || heldInputs.contains(sf::Keyboard::RShift);
}

bool WidgetEvents::isControlPressed() const
{
	return heldInputs.contains(sf::Keyboard::LControl) || heldInputs.contains(sf::Keyboard::RControl);
}

bool WidgetEvents::isAltPressed() const
{
	return heldInputs.contains(sf::Keyboard::LAlt) || heldInputs.contains(sf::Keyboard::RAlt);
}

bool WidgetEvents::isSystemPressed() const
{
	return heldInputs.contains(sf::Keyboard::LSystem) || heldInputs.contains(sf::Keyboard::RSystem);
}


} // namespace gui2
