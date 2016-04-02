#ifndef NULL_OUTPUT_HPP
#define NULL_OUTPUT_HPP

#include <iostream>

class NullOutputBuffer : public std::streambuf
{
public:

	std::streamsize xsputn(const char * s, std::streamsize n);
	int overflow(int c);
};

class NullOutputStream : public std::ostream
{
public:

	NullOutputStream() :
		std::ostream(&myBuffer)
	{}

private:

	NullOutputBuffer myBuffer;
};

NullOutputStream nullout;

#endif
