#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <SFML/Window.hpp>
#include <set>

#include "Shared/Utils/DataStream.hpp"

class Input {

private:

	enum {
		TypeIncr    = 0x10000000,	// difference between each input type.
		SysIncr     = 0x00000400	// offset for system keycodes.
	};

public:

	enum Type {
		None        = TypeIncr * 0,
		Keyboard    = TypeIncr * 1,
		Mouse       = TypeIncr * 2
	};
	enum MouseExtra {
		WheelUp     = 200,
		WheelDown   = 201
	};
	enum UnboundKey {
		Unbound     = 0
	};

	Input();
	explicit Input(sf::Int32 dataNum);
	explicit Input(sf::Event event);
	Input(sf::Keyboard::Key key);
	Input(sf::Mouse::Button key);
	Input(MouseExtra key);
	Input(UnboundKey key);

	// key name.
	std::string getName() const;

	// SFML-compatible code.
	sf::Int32 getCode() const;
	Type getType() const;

	// internally identifying code.
	sf::Int32 getDataNum() const;
	void setDataNum(sf::Int32 data);

	bool operator== (const Input & rhs) const;
	bool operator!= (const Input & rhs) const;
	bool operator< (const Input & rhs) const;

private:

	sf::Int32 myCode;

};

DataStream & operator<<(DataStream & strm, const Input & key);
DataStream & operator>>(DataStream & strm, Input & key);

class InputGroup {

public:

	InputGroup();
	InputGroup(Input key);

	void add(Input key);
	void remove(Input key);
	void clear();

	unsigned int getKeyCount() const;
	Input get(unsigned int index) const;
	bool contains(Input key) const;
	bool containsAnyOf(const InputGroup & input) const; // any input out of group can be used.
	bool containsAllOf(const InputGroup & input) const; // input combination containing group required.
	bool containsOnly(const InputGroup & input) const;  // specific input combination required.

private:

	std::vector<Input> myKeys;

};

DataStream & operator<<(DataStream & strm, const InputGroup & keyGroup);
DataStream & operator>>(DataStream & strm, InputGroup & keyGroup);

class InputProcessor {

public:

	// resets pressed keys.
	void reset();

	// reset internal state, must be called before event loop.
	void preProcess();

	// put this in the event processing loop of an sf::Window
	void processEvent(const sf::Event & event);

	// check if the key is being pressed.
	bool check(Input key) const;

	// check if any of the keys in the group are being pressed.
	bool check(InputGroup keyGroup) const;

private:

	std::set<Input> myPressedKeys;

};



#endif

