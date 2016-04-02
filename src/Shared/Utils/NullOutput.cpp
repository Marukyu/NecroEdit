#include "Shared/Utils/NullOutput.hpp"

std::streamsize NullOutputBuffer::xsputn(const char * s, std::streamsize n)
{
	return n;
}

int NullOutputBuffer::overflow(int c)
{
	return 1;
}
