#include "Shared/Utils/StringStream.hpp"
#include <iostream>

StringStream::StringStream()
{
	myStreamRedirect = 0;
}

StringStream::~StringStream()
{

}

StringStream::StringStream(std::ostream & redirect)
{
	myStreamRedirect = &redirect;
}


StringStream StringStream::Cout(std::cout);


std::string StringStream::str() const
{
	sf::Lock locky(myMutex);
	return myStream.str();
}

void StringStream::str(const std::string & data)
{
	sf::Lock locky(myMutex);
	myStream.str(data);
}

void StringStream::clear()
{
	sf::Lock locky(myMutex);
	myStream.clear();
}


