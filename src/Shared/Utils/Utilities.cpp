#include <SFML/Config.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <Shared/Utils/MiscMath.hpp>
#include <Shared/Utils/OSDetect.hpp>
#include <Shared/Utils/StrNumCon.hpp>
#include <Shared/Utils/Utilities.hpp>
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#ifdef WOS_LINUX
#	include <sys/stat.h>
#	include <sys/types.h>
#elif defined WOS_WINDOWS
#	include <shlobj.h>
#endif

std::string toUppercase(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
		*it = toupper(*it);

	return std::move(str);
}
std::string toLowercase(std::string str)
{
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
		*it = tolower(*it);

	return std::move(str);
}

std::string capitalize(std::string str)
{
	if (!str.empty())
		str[0] = toupper(str[0]);

	return std::move(str);
}

bool equalsIgnoreCase(const std::string & a, const std::string & b)
{
	std::size_t size = a.size();

	if (size != b.size())
	{
		return false;
	}

	for (std::size_t i = 0; i < size; ++i)
	{
		if (tolower(a[i]) != tolower(b[i]))
		{
			return false;
		}
	}
	return true;
}

void splitString(const std::string & str, const std::string & separator, std::vector<std::string> & results,
	bool ignoreEmpty)
{
	results.clear();

	std::size_t posA = 0;
	std::size_t posB = str.find_first_of(separator);

	while (posB != std::string::npos)
	{
		if (!ignoreEmpty || posA < posB)
		{
			results.emplace_back(str.data() + posA, str.data() + posB);
		}

		posA = posB + 1;
		posB = str.find_first_of(separator, posA);
	}

	if (posB == std::string::npos)
	{
		posB = str.size();
	}

	if (!ignoreEmpty || posA < posB)
	{
		results.emplace_back(str.data() + posA, str.data() + posB);
	}

	/*
	 results.clear();
	 std::size_t found = str.find_first_of(separator);

	 while (found != std::string::npos)
	 {
	 if (found > 0)
	 results.push_back(str.substr(0, found));
	 else if (!ignoreEmpty)
	 results.push_back(std::string());
	 str = str.substr(found + 1);
	 found = str.find_first_of(separator);
	 }
	 if (str.length() > 0)
	 results.push_back(str);
	 */
}

std::vector<std::string> splitString(const std::string & str, const std::string & separator, bool ignoreEmpty)
{
	std::vector<std::string> results;

	std::size_t posA = 0;
	std::size_t posB = str.find_first_of(separator);

	while (posB != std::string::npos)
	{
		if (!ignoreEmpty || posA < posB)
		{
			results.emplace_back(str.data() + posA, str.data() + posB);
		}

		posA = posB + 1;
		posB = str.find_first_of(separator, posA);
	}

	if (posB == std::string::npos)
	{
		posB = str.size();
	}

	if (!ignoreEmpty || posA < posB)
	{
		results.emplace_back(str.data() + posA, str.data() + posB);
	}

	return std::move(results);
}

std::vector<std::string> extractSection(const std::vector<std::string>& vector, const std::string& sectionName)
{
	std::vector<std::string> section;

	bool foundSection = false;

	for (const std::string & line : vector)
	{
		if (!line.empty() && line[0] == '[' && line.back() == ']')
		{
			if (foundSection)
			{
				break;
			}
			else if (line == "[" + sectionName + "]")
			{
				foundSection = true;
				continue;
			}
		}

		if (foundSection && !line.empty() && line[0] != '#')
		{
			section.push_back(line);
		}
	}

	return section;
}

std::string getTimeString()
{
	std::time_t rawtime;
	std::tm * timeinfo;
	char buffer[80];
	std::string ret;

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	ret.assign(buffer, strftime(buffer, 80, "%X", timeinfo));

	return ret;
}

bool fileExists(const std::string & filename)
{
	std::ifstream file;
	file.open(filename);
	return file.good();
}

bool createDirectory(const std::string & path)
{
#if defined WOS_LINUX || defined WOS_OSX

	// Check what kind of file is at the specified path.
	struct stat st;
	if (stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode))
	{
		// If it is a directory, simply return true.
		return true;
	}

	// Try to create directory, return false if any error occured.
	return mkdir(path.c_str(), S_IRWXU) == 0;

#elif defined WOS_WINDOWS

	// Create directory recursively.
	return SHCreateDirectoryEx(NULL, path.c_str(), NULL) == ERROR_SUCCESS;

#endif
}

bool createDirectoryStructure(const std::string & path)
{
#if defined WOS_LINUX || defined WOS_OSX

	std::size_t index = path.find_first_of('/');

	while (index != std::string::npos)
	{
		// Create parent directories.
		createDirectory(path.substr(0, index));
		index = path.find_first_of('/', index + 1);
	}

#endif

	// On Windows, createDirectory already works recursively.
	return createDirectory(path);
}

std::string readFile(const std::string & filename)
{
	std::string content;
	std::ifstream file(filename);
	if (!file)
	{
		return content;
	}
	file.seekg(0, std::ios::end);
	content.resize(file.tellg());
	file.seekg(0, std::ios::beg);
	file.read(&content[0], content.size());
	return content;
}

std::string getFileExtension(const std::string & filename)
{
	// no dot found in file name.
	if (filename.find_last_of(".") == std::string::npos)
	{
		return "";
	}

	std::string extension = filename.substr(filename.find_last_of("."));

	// last dot found within path name, so no dot in file name.
	if (extension.find_first_of("/\\") != std::string::npos)
	{
		return "";
	}

	return extension;
}
std::string removeFileExtension(const std::string & filename)
{
	// no dot found in file name.
	if (filename.find_last_of(".") == std::string::npos)
	{
		return filename;
	}

	std::string filestem = filename.substr(0, filename.find_last_of("."));
	std::string extension = filename.substr(filename.find_last_of("."));

	// last dot found within path name, no dot to be removed in file name.
	if (extension.find_first_of("/\\") != std::string::npos)
	{
		return filename;
	}

	return filestem;
}

std::string getFilePath(const std::string& filename)
{
	std::size_t index = filename.find_last_of("/\\");

	if (index == std::string::npos)
	{
		return "";
	}

	return filename.substr(0, index);
}

std::string removeFilePath(const std::string& filename)
{
	std::size_t index = filename.find_last_of("/\\");

	if (index == std::string::npos)
	{
		return filename;
	}

	return filename.substr(index + 1);
}

std::string getByteSizeString(sf::Uint64 bytes)
{
	if (bytes < 1000)
	{
		return cNtoS(bytes) + " B";
	}
	else if (bytes < 1000000)
	{
		return cNtoS(round<double>(bytes / 1000.0, 1)) + " KB";
	}
	else if (bytes < 1000000000)
	{
		return cNtoS(round<double>(bytes / 1000000.0, 1)) + " MB";
	}
	else
	{
		return cNtoS(round<double>(bytes / 1000000000.0, 1)) + " GB";
	}
}

std::string fillStringWithChar(std::string str, char chr, std::size_t targetSize)
{
	if (str.size() < targetSize)
		return std::string(targetSize - str.size(), chr) + str;
	else
		return str;
}

std::string getSfTimeString(sf::Time time)
{
	std::ostringstream str;

	str.precision(2);
	str << std::fixed;

	if (time < sf::milliseconds(1))
	{
		str << time.asMicroseconds() << "us";
	}
	else if (time < sf::seconds(1))
	{
		str << round(time.asSeconds() * 1000.f, 2) << "ms";
	}
	else if (time < sf::seconds(60))
	{
		str << round(time.asSeconds(), 2) << "s";
	}
	else if (time < sf::seconds(60 * 60))
	{
		str << int(time.asSeconds()) / 60 << ":" << fillStringWithChar(cNtoS(int(time.asSeconds()) % 60), '0', 2) << "."
			<< fillStringWithChar(cNtoS((time.asMilliseconds() / 10) % 100), '0', 2);
	}
	else
	{
		return getRoughSfTimeString(time);
	}

	return str.str();
}

std::string getRoughSfTimeString(sf::Time time)
{
	if (time > sf::seconds(60 * 60))
	{
		return cNtoS(int(time.asSeconds() / 60 / 60)) + ":"
			+ fillStringWithChar(cNtoS(int(time.asSeconds() / 60) % 60), '0', 2) + ":"
			+ fillStringWithChar(cNtoS(int(time.asSeconds()) % 60), '0', 2);
	}
	else
	{
		return cNtoS(int(time.asSeconds() / 60) % 60) + ":"
			+ fillStringWithChar(cNtoS(int(time.asSeconds()) % 60), '0', 2);
	}
}

bool pointInPoly(const std::vector<sf::Vector2f> & vertices, const sf::Vector2f & point)
{
	bool ret = false;
	for (std::size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++)
	{
		const sf::Vector2f & v1 = vertices[i], &v2 = vertices[j];
		if ((v1.y > point.y) != (v2.y > point.y) && point.x < (v2.x - v1.x) * (point.y - v1.y) / (v2.y - v1.y) + v1.x)
			ret = !ret;
	}
	return ret;
}

void plotBresenham(int x1, int y1, int x2, int y2, std::function<void(int, int)> callback)
{
	bool steep = (std::abs(y2 - y1) > std::abs(x2 - x1));

	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	int dx = x2 - x1;
	int dy = std::abs(y2 - y1);

	int error = dx;
	int ystep = (y1 < y2) ? 1 : -1;
	int y = y1;

	for (int x = x1; x <= x2; ++x)
	{
		if (steep)
		{
			callback(y, x);
		}
		else
		{
			callback(x, y);
		}

		error -= dy * 2;
		if (error < 0)
		{
			y += ystep;
			error += dx * 2;
		}
	}
}
