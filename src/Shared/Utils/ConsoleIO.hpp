#ifndef CONSOLE_IO_HPP
#define CONSOLE_IO_HPP

#include <string>

class Console {

public:

/*
	static void write(const std::string & text);
	static void writeLine(const std::string & text);
*/

	static bool hasLine();
	static std::string getLine();

	static void setTitle(const std::string & title);

	enum Color {

		Default = -1,
		White = 0,
		Red,
		Green,
		Blue,
		Cyan,
		Magenta,
		Yellow,
		Black,
		Grey

	};

	static void setColor(Color color, bool bright = false);
	static void setBackColor(Color color, bool bright = false);

private:

	static void init();

	static bool initialized;

};

#endif

