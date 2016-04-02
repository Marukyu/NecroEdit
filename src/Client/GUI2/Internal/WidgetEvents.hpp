#ifndef GUI2_INTERNAL_WIDGET_EVENTS_HPP
#define GUI2_INTERNAL_WIDGET_EVENTS_HPP

#include "Client/Game/Input.hpp"

#include <SFML/Graphics/Transform.hpp>

namespace gui2
{


/**
 * holds information about the mouse cursor and keypresses.
 * passed by const-reference to containers during processing.
 * created once per frame within each instance.
 */
class ContainerEvents
{
public:

	/// holds a single mouse button press or release.
	struct MouseEvent
	{
		MouseEvent(bool isPress, sf::Vector2f position, Input button) :
			isPress(isPress),
			position(position),
			button(button)
		{}

		bool isPress;
		sf::Vector2f position;
		Input button;
	};

	/// holds a single key press or release
	struct KeyEvent
	{
		KeyEvent(bool isPress, Input code) :
			isPress(isPress),
			code(code)
		{}

		bool isPress;
		Input code;
	};

	/// current absolute position of the mouse cursor.
	sf::Vector2f mousePosition;

	/// currently held keys and mouse buttons.
	InputGroup heldInputs;

	/// just pressed keys and mouse buttons.
	InputGroup pressedInputs;

	/// newly input text characters.
	std::string textInput;

	/// mouse wheel movement amount.
	int mouseWheelDelta;


	/// newly pressed/released mouse buttons. forward iteration == old -> new.
	std::vector<MouseEvent> mouseEvents;

	/// newly pressed/released keys. forward iteration == old -> new.
	std::vector<KeyEvent> keyEvents;

private:

	// not intended for public use.
	ContainerEvents() :
		mousePosition(),
		heldInputs(),
		textInput(),
		mouseWheelDelta(0),
		mouseEvents(),
		keyEvents()
	{}

	// reset before event loop.
	void reset();

	// automatically process events from an sf::Window.
	void processEvent(const sf::Event & event);

	friend class Interface;
};


/**
 * holds information about the mouse cursor and keypresses,
 * as well as widget focus for mouse or keyboard input.
 * passed by const-reference to widgets during processing.
 * mouse position is transformed according to each widget's position data.
 */
class WidgetEvents
{
public:

	/// current position of the mouse cursor relative to the widget.
	sf::Vector2f mousePosition;

	/// currently held keys and mouse buttons.
	const InputGroup & heldInputs;

	/// just pressed keys and mouse buttons.
	const InputGroup & pressedInputs;

	/// newly input text characters.
	const std::string & textInput;

	/// mouse wheel movement amount.
	int mouseWheelDelta;

	/// true if mouse/keyboard input should be registered by this widget.
	bool mouseFocus;
	bool keyboardFocus;


	/// check if specific modifier keys are held down.
	bool isShiftPressed() const;
	bool isControlPressed() const;
	bool isAltPressed() const;
	bool isSystemPressed() const;

private:

	/// constructs widget events from container events and a transformed mouse position.
	WidgetEvents(const ContainerEvents & events, const sf::Transform & transform, bool mouseFocus, bool keyboardFocus) :
		mousePosition(transform.getInverse().transformPoint(events.mousePosition)),
		heldInputs(events.heldInputs),
		pressedInputs(events.pressedInputs),
		textInput(events.textInput),
		mouseWheelDelta(events.mouseWheelDelta),
		mouseFocus(mouseFocus),
		keyboardFocus(keyboardFocus),
		transform(transform),
		conEvents(events)
	{}

	// not intended for public use.
	WidgetEvents() = delete;

	// for computation of transformed mouse positions.
	const sf::Transform & transform;

	// for data retrieval in processContainer().
	const ContainerEvents & conEvents;

	friend class Interface;
	friend class Container;
	friend class Widget;
};

/**
 * passed by const-reference to containers and widgets during processing.
 */
struct WidgetFlags
{
};

} // namespace gui2

#endif
