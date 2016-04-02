#ifndef STR_NUM_CON_HPP
#define STR_NUM_CON_HPP

#include <SFML/Config.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <sstream>

template<class T>
bool str2Num(T& num, const std::string& str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	std::istringstream istrm(str);
	return !(istrm >> format >> num).fail();
}

template<class T>
bool num2Str(const T& num, std::string& str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	std::ostringstream ostrm;
	if ((ostrm << format << num).fail())
		return false;
	str = ostrm.str();
	return true;
}

// hex character functions...

inline sf::Int8 hex2Num(char hex)
{
	if (hex >= '0' && hex <= '9')
		return hex - '0';
	if (hex >= 'A' && hex <= 'F')
		return hex - 'A' + 0xA;
	if (hex >= 'a' && hex <= 'f')
		return hex - 'a' + 0xA;
	else
		return 0;
}

inline char num2Hex(sf::Int8 num)
{
	if (num >= 0 && num <= 9)
		return '0' + num;
	if (num >= 0xA && num <= 0xF)
		return num + 'A' - 0xA;
	else
		return '0';
}

// utility functions...

template<class T>
std::string cNtoS(T num, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	std::string str;
	num2Str(num, str, format);
	return str;
}

template<>
inline std::string cNtoS<>(char num, std::ios_base& (*format)(std::ios_base&))
{
	std::string str;
	num2Str((int) num, str, format);
	return str;
}

template<>
inline std::string cNtoS<>(unsigned char num, std::ios_base& (*format)(std::ios_base&))
{
	std::string str;
	num2Str((int) num, str, format);
	return str;
}

template<typename TVec>
std::string cNtoS(sf::Vector2<TVec> vec, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	std::string str1, str2;
	num2Str(vec.x, str1, format);
	num2Str(vec.y, str2, format);
	return str1 + ";" + str2;
}
/*
 inline std::string cNtoS(sf::Uint8 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 // we need a special case for unsigned chars.
 std::string str;
 num2Str((sf::Int16)num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Uint16 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Uint32 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Uint64 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Int8 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Int16 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Int32 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(sf::Int64 num, std::ios_base& (*format)(std::ios_base&) = std::dec)
 {
 std::string str;
 num2Str(num, str, format);
 return str;
 }
 inline std::string cNtoS(float num)
 {
 std::string str;
 num2Str(num, str, std::dec);
 return str;
 }
 inline std::string cNtoS(double num)
 {
 std::string str;
 num2Str(num, str, std::dec);
 return str;
 }
 */

template<typename T>
inline T cStoN(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	T num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}

template<typename TVec>
sf::Vector2<TVec> cStoV(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	TVec num1, num2;
	std::string::size_type found = str.find_first_of(';');

	if (found == std::string::npos || found == str.size() - 1)
		return sf::Vector2<TVec>();

	if (!str2Num(num1, str.substr(0, found), format))
		return sf::Vector2<TVec>();
	if (!str2Num(num2, str.substr(found + 1), format))
		return sf::Vector2<TVec>();
	return sf::Vector2<TVec>(num1, num2);
}

inline sf::Uint8 cStoUB(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	// Workaround for stringstream char behavior.
	sf::Int16 num;
	if (str2Num(num, str, format))
		return (sf::Uint8) num;
	else
		return 0;
}
inline sf::Uint16 cStoUS(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	sf::Uint16 num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}
inline sf::Uint32 cStoUI(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	sf::Uint32 num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}
inline sf::Uint64 cStoUL(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	sf::Uint64 num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}
inline sf::Int8 cStoB(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	// Workaround for stringstream char behavior.
	sf::Int16 num;
	if (str2Num(num, str, format))
		return (sf::Int8) num;
	else
		return 0;
}
inline sf::Int16 cStoS(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	sf::Int16 num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}
inline sf::Int32 cStoI(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	sf::Int32 num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}
inline sf::Int64 cStoL(std::string str, std::ios_base& (*format)(std::ios_base&) = std::dec)
{
	sf::Int64 num;
	if (str2Num(num, str, format))
		return num;
	else
		return 0;
}
inline float cStoF(std::string str)
{
	float num;
	if (str2Num(num, str))
		return num;
	else
		return 0.f;
}
inline double cStoD(std::string str)
{
	double num;
	if (str2Num(num, str))
		return num;
	else
		return 0.0;
}

#endif
