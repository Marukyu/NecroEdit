#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <string>
#include <vector>
#include <functional>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

static const std::string stringAlphabet = "abcdefghijklmnopqrstuvwxyz";
std::string toUppercase(std::string str);
std::string toLowercase(std::string str);
std::string capitalize(std::string str);

bool equalsIgnoreCase(const std::string & a, const std::string & b);

//void splitString(const std::string & str, const std::string & separator, std::vector<std::string> & results, bool ignoreEmpty = false);
std::vector<std::string> splitString(const std::string & str, const std::string & separator, bool ignoreEmpty = false);

/**
 * Extracts a specific section from an ini-like vector.
 * 
 * The entries with the format "[sectionName]" will be used as delimiters.
 */
std::vector<std::string> extractSection(const std::vector<std::string> & vector, const std::string & sectionName);

std::string getTimeString();

// TODO: Place these functions in separate FileSystem namespace / header file.
bool fileExists(const std::string & filename);

bool createDirectory(const std::string & path);
bool createDirectoryStructure(const std::string & path);

std::string readFile(const std::string & filename);

std::string getFileExtension(const std::string & filename);
std::string removeFileExtension(const std::string & filename);
std::string getFilePath(const std::string & filename);
std::string removeFilePath(const std::string & filename);

template <typename T>
sf::Rect<T> moveRect(sf::Rect<T> rect, sf::Vector2<T> off)
{
	rect.left += off.x;
	rect.top += off.y;
	return rect;
}
template <typename T>
sf::Rect<T> expandRect(sf::Rect<T> rect, sf::Vector2<T> exp)
{
	rect.left -= exp.x;
	rect.top -= exp.y;
	rect.width += exp.x * (T)2;
	rect.height += exp.y * (T)2;
	return rect;
}

std::string getByteSizeString(sf::Uint64 bytes);
std::string getSfTimeString(sf::Time time);
std::string getRoughSfTimeString(sf::Time time);



template <typename T>
sf::Rect<T> rectIntersect(sf::Rect<T> a, sf::Rect<T> b)
{
	T a_right  = a.left + a.width, a_bottom = a.top + a.height;
	T b_right  = b.left + b.width, b_bottom = b.top + b.height;

	T r_left   = std::max(a.left,   b.left  );
	T r_top	= std::max(a.top,	b.top   );
	T r_right  = std::min(a_right,  b_right );
	T r_bottom = std::min(a_bottom, b_bottom);

	return sf::Rect<T>(r_left, r_top, r_right - r_left, r_bottom - r_top);
}

bool pointInPoly(const std::vector<sf::Vector2f> & vertices, const sf::Vector2f & point);

void plotBresenham(int x1, int y1, int x2, int y2, std::function<void(int, int)> callback);

#endif
