#include "Client/Game/Input.hpp"
#include "Shared/Utils/StrNumCon.hpp"
#include "Shared/Utils/OSDetect.hpp"

#include <iostream>

#ifdef WOS_MODIFIED_SFML
	#ifdef WOS_LINUX
		#include <X11/Xlib.h>
		#undef None	 // sneaky macro "None" in X.h conflicts with keybind constants.
	#elif defined WOS_WINDOWS
		#include <windows.h>
	#endif
#endif

Input::Input()
{
	myCode = None + Unbound;
}
Input::Input(sf::Int32 dataNum)
{
	myCode = dataNum;
}
Input::Input(sf::Event event)
{
	if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
	{
		if (event.key.code == sf::Keyboard::Unknown)
		{
#ifdef WOS_MODIFIED_SFML
			if (event.key.syscode != 0)
			{
				myCode = Keyboard + SysIncr + event.key.syscode;
			}
			else
#endif
				myCode = None + Unbound;
		}
		else
			myCode = Keyboard + event.key.code;
	}

	else if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased)
	{
#ifdef WOS_MODIFIED_SFML
		if (event.mouseButton.button == sf::Mouse::Unknown)
		{
			myCode = Mouse + SysIncr + event.mouseButton.syscode;
		}
		else
#endif
			myCode = Mouse + event.mouseButton.button;
	}

	else if (event.type == sf::Event::MouseWheelMoved && event.mouseWheel.delta > 0)
		myCode = Mouse + WheelUp;
	else if (event.type == sf::Event::MouseWheelMoved && event.mouseWheel.delta < 0)
		myCode = Mouse + WheelDown;

	else
		myCode = None + Unbound;
}
Input::Input(sf::Keyboard::Key key)
{
	myCode = Keyboard + key;
}
Input::Input(sf::Mouse::Button key)
{
	myCode = Mouse + key;
}
Input::Input(MouseExtra key)
{
	myCode = Mouse + key;
}
Input::Input(UnboundKey key)
{
	myCode = None + key;
}

std::string Input::getName() const
{
	if (getType() == Keyboard)
	{
		switch (getCode())
		{
			case sf::Keyboard::A:			return "A";
			case sf::Keyboard::B:			return "B";
			case sf::Keyboard::C:			return "C";
			case sf::Keyboard::D:			return "D";
			case sf::Keyboard::E:			return "E";
			case sf::Keyboard::F:			return "F";
			case sf::Keyboard::G:			return "G";
			case sf::Keyboard::H:			return "H";
			case sf::Keyboard::I:			return "I";
			case sf::Keyboard::J:			return "J";
			case sf::Keyboard::K:			return "K";
			case sf::Keyboard::L:			return "L";
			case sf::Keyboard::M:			return "M";
			case sf::Keyboard::N:			return "N";
			case sf::Keyboard::O:			return "O";
			case sf::Keyboard::P:			return "P";
			case sf::Keyboard::Q:			return "Q";
			case sf::Keyboard::R:			return "R";
			case sf::Keyboard::S:			return "S";
			case sf::Keyboard::T:			return "T";
			case sf::Keyboard::U:			return "U";
			case sf::Keyboard::V:			return "V";
			case sf::Keyboard::W:			return "W";
			case sf::Keyboard::X:			return "X";
			case sf::Keyboard::Y:			return "Y";
			case sf::Keyboard::Z:			return "Z";
			case sf::Keyboard::Num0:		return "0";
			case sf::Keyboard::Num1:		return "1";
			case sf::Keyboard::Num2:		return "2";
			case sf::Keyboard::Num3:		return "3";
			case sf::Keyboard::Num4:		return "4";
			case sf::Keyboard::Num5:		return "5";
			case sf::Keyboard::Num6:		return "6";
			case sf::Keyboard::Num7:		return "7";
			case sf::Keyboard::Num8:		return "8";
			case sf::Keyboard::Num9:		return "9";
			case sf::Keyboard::Escape:		return "Escape";
			case sf::Keyboard::LControl:	return "Left control";
			case sf::Keyboard::LShift:		return "Left shift";
			case sf::Keyboard::LAlt:		return "Left alt";
			case sf::Keyboard::LSystem:		return "Left system";
			case sf::Keyboard::RControl:	return "Right control";
			case sf::Keyboard::RShift:		return "Right shift";
			case sf::Keyboard::RAlt:		return "Right alt";
			case sf::Keyboard::RSystem:		return "Right system";
			case sf::Keyboard::Menu:		return "Menu";
			case sf::Keyboard::LBracket:	return "[";
			case sf::Keyboard::RBracket:	return "]";
			case sf::Keyboard::SemiColon:	return ";";
			case sf::Keyboard::Comma:		return ",";
			case sf::Keyboard::Period:		return ".";
			case sf::Keyboard::Quote:		return "\"";
			case sf::Keyboard::Slash:		return "/";
			case sf::Keyboard::BackSlash:	return "\\";
			case sf::Keyboard::Tilde:		return "~";
			case sf::Keyboard::Equal:		return "=";
			case sf::Keyboard::Dash:		return "-";
			case sf::Keyboard::Space:		return "Space";
			case sf::Keyboard::Return:		return "Enter";
			case sf::Keyboard::BackSpace:	return "Erase";
			case sf::Keyboard::Tab:			return "Tab";
			case sf::Keyboard::PageUp:		return "Page up";
			case sf::Keyboard::PageDown:	return "Page down";
			case sf::Keyboard::End:			return "End";
			case sf::Keyboard::Home:		return "Pos 1";
			case sf::Keyboard::Insert:		return "Insert";
			case sf::Keyboard::Delete:		return "Delete";
			case sf::Keyboard::Add:			return "Num +";
			case sf::Keyboard::Subtract:	return "Num -";
			case sf::Keyboard::Multiply:	return "Num *";
			case sf::Keyboard::Divide:		return "Num /";
			case sf::Keyboard::Left:		return "Left";
			case sf::Keyboard::Right:		return "Right";
			case sf::Keyboard::Up:			return "Up";
			case sf::Keyboard::Down:		return "Down";
			case sf::Keyboard::Numpad0:		return "Numpad 0";
			case sf::Keyboard::Numpad1:		return "Numpad 1";
			case sf::Keyboard::Numpad2:		return "Numpad 2";
			case sf::Keyboard::Numpad3:		return "Numpad 3";
			case sf::Keyboard::Numpad4:		return "Numpad 4";
			case sf::Keyboard::Numpad5:		return "Numpad 5";
			case sf::Keyboard::Numpad6:		return "Numpad 6";
			case sf::Keyboard::Numpad7:		return "Numpad 7";
			case sf::Keyboard::Numpad8:		return "Numpad 8";
			case sf::Keyboard::Numpad9:		return "Numpad 9";
			case sf::Keyboard::F1:			return "F1";
			case sf::Keyboard::F2:			return "F2";
			case sf::Keyboard::F3:			return "F3";
			case sf::Keyboard::F4:			return "F4";
			case sf::Keyboard::F5:			return "F5";
			case sf::Keyboard::F6:			return "F6";
			case sf::Keyboard::F7:			return "F7";
			case sf::Keyboard::F8:			return "F8";
			case sf::Keyboard::F9:			return "F9";
			case sf::Keyboard::F10:			return "F10";
			case sf::Keyboard::F11:			return "F11";
			case sf::Keyboard::F12:			return "F12";
			case sf::Keyboard::F13:			return "F13";
			case sf::Keyboard::F14:			return "F14";
			case sf::Keyboard::F15:			return "F15";
			case sf::Keyboard::Pause:		return "Pause";

#ifdef WOS_MODIFIED_SFML
	#ifdef WOS_LINUX
			default:
			{
				const char * keysymName = XKeysymToString(getCode() - SysIncr);
				return keysymName ? keysymName : "Key " + cNtoS(getCode() - SysIncr);
			}
	#else
			default:					  return "Key " + cNtoS(getCode() - SysIncr);
	#endif
#endif
		}
	}
	else if (getType() == Mouse)
	{
		switch (getCode())
		{
			case sf::Mouse::Left:			return "Left click";
			case sf::Mouse::Right:			return "Right click";
			case sf::Mouse::Middle:			return "Middle click";
			case sf::Mouse::XButton1:		return "Side click 1";
			case sf::Mouse::XButton2:		return "Side click 2";
			case WheelUp:					return "Scroll up";
			case WheelDown:					return "Scroll down";

			default:					return "Mouse button " + cNtoS(getCode() - SysIncr);
		}
	}
	else if (getType() == None)
	{
		return "- none -";
	}
	else
	{
		return "???";
	}

}

sf::Int32 Input::getCode() const
{
	return myCode % TypeIncr;
}
Input::Type Input::getType() const
{
	return (Type)((myCode / TypeIncr) * TypeIncr);
}

sf::Int32 Input::getDataNum() const
{
	return myCode;
}
void Input::setDataNum(sf::Int32 data)
{
	myCode = data;
}

bool Input::operator== (const Input & rhs) const
{
	return getDataNum() == rhs.getDataNum();
}
bool Input::operator!= (const Input & rhs) const
{
	return getDataNum() != rhs.getDataNum();
}
bool Input::operator< (const Input & rhs) const
{
	return getDataNum() < rhs.getDataNum();
}

InputGroup::InputGroup()
{
}
InputGroup::InputGroup(Input key)
{
	add(key);
}
void InputGroup::add(Input key)
{
	// check if key is already in group...
	for (std::vector<Input>::iterator it = myKeys.begin(); it != myKeys.end(); ++it)
	{
		if (*it == key)
			return;
	}
	myKeys.push_back(key);
}
void InputGroup::remove(Input key)
{
	for (std::vector<Input>::iterator it = myKeys.begin(); it != myKeys.end(); ++it)
	{
		if (*it == key)
		{
			myKeys.erase(it);
			return;
		}
	}
}
void InputGroup::clear()
{
	myKeys.clear();
}

unsigned int InputGroup::getKeyCount() const
{
	return myKeys.size();
}
Input InputGroup::get(unsigned int index) const
{
	if (index < myKeys.size())
		return myKeys[index];
	else
		return Input();
}
bool InputGroup::contains(Input key) const
{
	for (std::vector<Input>::const_iterator it = myKeys.begin(); it != myKeys.end(); ++it)
	{
		if (*it == key)
			return true;
	}
	return false;
}
bool InputGroup::containsAnyOf(const InputGroup & input) const
{
	for (std::vector<Input>::const_iterator it = myKeys.begin(); it != myKeys.end(); ++it)
	{
		if (input.contains(*it))
			return true;
	}
	return false;
}
bool InputGroup::containsAllOf(const InputGroup & input) const
{
	for (std::vector<Input>::const_iterator it = myKeys.begin(); it != myKeys.end(); ++it)
	{
		if (!input.contains(*it))
			return false;
	}
	return true;
}
bool InputGroup::containsOnly(const InputGroup & input) const
{
	return containsAllOf(input) && input.getKeyCount() == getKeyCount();
}

void InputProcessor::reset()
{
	myPressedKeys.clear();
}

void InputProcessor::preProcess()
{
	// clear mouse wheel state.
	myPressedKeys.erase(Input::WheelUp);
	myPressedKeys.erase(Input::WheelDown);
}

void InputProcessor::processEvent(const sf::Event & event)
{
	Input key(event);
	if (key.getType() == Input::None)  // ignore bad events.
		return;

	// 'down' event?
	if (event.type == sf::Event::KeyPressed || event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseWheelMoved)
		myPressedKeys.insert(key);

	// 'up' event?
	else if (event.type == sf::Event::KeyReleased || event.type == sf::Event::MouseButtonReleased)
		myPressedKeys.erase(key);
}

bool InputProcessor::check(Input key) const
{
	return myPressedKeys.find(key) != myPressedKeys.end();
}

bool InputProcessor::check(InputGroup keyGroup) const
{
	for (unsigned int i = 0; i < keyGroup.getKeyCount(); ++i)
		if (check(keyGroup.get(i))) return true;

	return false;
}
