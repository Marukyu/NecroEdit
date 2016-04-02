#ifndef STRING_STREAM_HPP
#define STRING_STREAM_HPP

#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <sstream>

class StringStream
{
public:

	StringStream();
	~StringStream();

	template <typename T>
	StringStream & operator<< (T data);

	std::string str() const;
	void str(const std::string & data);

	void clear();

	static StringStream Cout;

private:

	StringStream(std::ostream & redirect);

	mutable sf::Mutex myMutex;
	std::ostringstream myStream;
	std::ostream * myStreamRedirect;
};

template <typename T>
StringStream & StringStream::operator<<(T data)
{
	if (myStreamRedirect)
	{
		sf::Lock locky(myMutex);
		(*myStreamRedirect) << data;
	}
	else
	{
		sf::Lock locky(myMutex);
		myStream << data;
	}
	return *this;
}

#endif

