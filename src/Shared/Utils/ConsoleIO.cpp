#include "Shared/Utils/ConsoleIO.hpp"
#include "Shared/Utils/OSDetect.hpp"

bool Console::initialized = false;

#ifdef WOS_WINDOWS

#include <conio.h>

void Console::init()
{

}

std::string Console::getLine()
{
	return "";
}

bool Console::hasLine()
{
	return false;
}

void Console::setTitle(const std::string & title)
{

}

void Console::setColor(Color color, bool bright)
{

}

void Console::setBackColor(Color color, bool bright)
{

}
#elif defined WOS_LINUX

#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <vector>

#include "Shared/Utils/Utilities.hpp"

namespace priv
{
std::vector<char> inputBuffer(4096);

bool hasChar()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}
}

using namespace priv;

void Console::init()
{
	if (initialized)
	{
		return;
	}

	initialized = true;
}

/*
 void Console::write(const std::string& text)
 {
 init();

 std::vector<std::string> lines;
 splitString(text, "\n", lines);

 if (lines.empty())
 return;

 // write first line.
 if (!lines[0].empty())
 std::cout.write(lines[0].data(), lines[0].size());

 if (lines.size() > 1)
 {
 // erase input buffer characters.
 static std::string lineEraser("\033[2K");
 std::cout.write(lineEraser.data(), lineEraser.size());

 // handle newlines.
 for (std::size_t i = 1; i < lines.size(); ++i)
 {
 std::cout.put('\n');
 std::cout.write(lines[i].data(), lines[i].size());
 }

 // replace input buffer characters.
 std::cout.write(inputBuffer.data(), inputBuffer.size());
 std::cout.put('\n');
 }
 }

 void Console::writeLine(const std::string& text)
 {
 write(text);
 write("\n");
 }
 */

bool Console::hasLine()
{
	return hasChar();

	/*
	 init();

	 // check if input buffer was modified.
	 while (hasChar())
	 {
	 char chr = std::cin.get();

	 switch (chr)
	 {
	 case '\n':
	 lineBuffer.push(std::string(inputBuffer.data(), inputBuffer.size()));
	 inputBuffer.clear();
	 break;

	 case '\b':
	 inputBuffer.pop_back();
	 break;

	 default:
	 inputBuffer.push_back(chr);
	 }
	 }

	 return !lineBuffer.empty();
	 */
}

std::string Console::getLine()
{
	std::cin.getline(inputBuffer.data(), inputBuffer.size());
	return std::string(inputBuffer.data(), std::cin.gcount() - 1);

	/*
	 if (lineBuffer.empty())
	 {
	 return std::string();
	 }
	 else
	 {
	 std::string ret;
	 ret = std::move(lineBuffer.front());
	 lineBuffer.pop();
	 return std::move(ret);
	 }
	 */
}

void Console::setTitle(const std::string & title)
{
	std::cout << "\033]0;" << title << "\007";
}

void Console::setColor(Color color, bool bright)
{
	if (bright)
	{
		std::cout << "\033[1;";
	}
	else
	{
		std::cout << "\033[22;";
	}

	switch (color)
	{
	case Default:
		std::cout << "39m";
		break;
	case White:
	case Grey:
		std::cout << "37m";
		break;
	case Red:
		std::cout << "31m";
		break;
	case Green:
		std::cout << "32m";
		break;
	case Blue:
		std::cout << "34m";
		break;
	case Cyan:
		std::cout << "36m";
		break;
	case Magenta:
		std::cout << "35m";
		break;
	case Yellow:
		std::cout << "33m";
		break;
	case Black:
		std::cout << "30m";
		break;
	default:
		break;
	}
}

void Console::setBackColor(Color color, bool bright)
{
	/*
	 if (bright)
	 {
	 std::cout << "\033[1;";
	 }
	 else
	 {
	 std::cout << "\033[22;";
	 }
	 */
	std::cout << "\033[";

	switch (color)
	{
	case Default:
		std::cout << "49m";
		break;
	case White:
	case Grey:
		std::cout << "47m";
		break;
	case Red:
		std::cout << "41m";
		break;
	case Green:
		std::cout << "42m";
		break;
	case Blue:
		std::cout << "44m";
		break;
	case Cyan:
		std::cout << "46m";
		break;
	case Magenta:
		std::cout << "45m";
		break;
	case Yellow:
		std::cout << "43m";
		break;
	case Black:
		std::cout << "40m";
		break;
	default:
		break;
	}
}

#endif
