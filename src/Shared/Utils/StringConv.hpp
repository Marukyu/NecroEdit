#ifndef STRING_CONV_HPP
#define STRING_CONV_HPP

#include <string>
#include <sstream>

#include <SFML/System/Vector2.hpp>

struct StringConvUtil
{
	template <class T>
	static bool str2Num(T& num, const std::string& str, std::ios_base& (*format)(std::ios_base&) = std::dec)
	{
		istrm.str(str);
		istrm.clear();
		return !(istrm >> format >> num).fail();
	}

	template <class T>
	static bool num2Str(const T& num, std::string& str, std::ios_base& (*format)(std::ios_base&) = std::dec)
	{
		ostrm.str("");
		if ((ostrm << format << num).fail())
			return false;
		str = ostrm.str();
		return true;
	}

private:

	static std::istringstream istrm;
	static std::ostringstream ostrm;
};

template<typename T>
struct StringConv
{
	static std::string toString(T value);
	static T fromString(const std::string & str);
};

template<typename TVec>
struct StringConv<sf::Vector2<TVec> >
{
	static std::string toString(sf::Vector2<TVec> value)
	{
		std::string str1, str2;
		StringConvUtil::num2Str<TVec>(value.x, str1);
		StringConvUtil::num2Str<TVec>(value.y, str2);
		return str1 + ";" + str2;
	}
	static sf::Vector2<TVec> fromString(const std::string & str)
	{
		TVec num1, num2;
		std::string::size_type found = str.find_first_of(';');

		if (found == std::string::npos || found == str.size() - 1)
			return sf::Vector2<TVec>();

		StringConvUtil::str2Num<TVec>(num1, str.substr(0, found));
		StringConvUtil::str2Num<TVec>(num2, str.substr(found + 1));
		return sf::Vector2<TVec>(num1, num2);
	}
};

template<typename T>
std::string StringConv<T>::toString(T value)
{
	std::string str;
	StringConvUtil::num2Str<T>(value, str);
	return str;
}
template<typename T>
T StringConv<T>::fromString(const std::string & str)
{
	T num;
	StringConvUtil::str2Num<T>(num, str);
	return num;
}

#endif
