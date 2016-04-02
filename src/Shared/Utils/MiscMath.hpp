#ifndef MISC_MATH_HPP
#define MISC_MATH_HPP

#include <climits>
#include <cmath>
#include <cstdlib>

#include <SFML/System/Vector2.hpp>

/// returns the absolute value of the given integer.
template <typename T>
inline T iAbs(T num)
{
	if (num < 0)
		return -num;
	else
		return num;
}

template <typename T>
inline T round(T r, unsigned int d)
{
	T off = 1.0;
	for (unsigned int i = 0; i < d; ++i)
		off *= 10.0;
	return (T)((int)(r*off + (r < 0.0 ? -0.5 : 0.5))) / off;
}

/*
inline int sgnMod(int a, int b)
{
	int ret = a % b;
	if (ret < 0) ret = b - ret;
	return ret;
}
*/

template<typename T>
inline T sgnMod(T a, T b)
{
	return (a % b + b) % b;
}

template<typename T>
inline T sgnFltMod(T a, T b)
{
	return std::fmod(std::fmod(a, b) + b, b);
}

template<typename T>
inline T floorDivP(T a, T b)
{
	return (a < 0) ? (a - b + 1) / b : (a / b);
}

template <typename T>
inline T clamp(T min, T num, T max)
{
	return num < max ? (num > min ? num : min) : max;
}


template <typename T>
inline T interpolateLinear(T a, T b, float val)
{
	return a * (1.f-val) + b * val;
}

/// returns a random floating point number between 0.0 and 1.0.
inline float randFloat()
{
	return (float)std::rand()/(float)RAND_MAX;
}

/// returns a random floating point number in a given interval.
inline float randFloat(float min, float max)
{
	return ((float)std::rand()/(float)RAND_MAX)*(max-min)+min;
}

inline sf::Vector2f middlePoint(sf::Vector2f a, sf::Vector2f b)
{
	return sf::Vector2f((a.x + b.x)/2.0, (a.y + b.y)/2.0);
}

inline sf::Vector2f middlePoint(sf::Vector2f a, sf::Vector2f b, float a_fac)
{
	return sf::Vector2f((a.x * a_fac + b.x)/(a_fac+1), (a.y * a_fac + b.y)/(a_fac+1));
}

inline float vectorSquareLength(sf::Vector2f vec)
{
	return vec.x * vec.x + vec.y * vec.y;
}

inline float vectorLength(sf::Vector2f vec)
{
	return std::sqrt(vectorSquareLength(vec));
}

inline sf::Vector2f normalize(sf::Vector2f vec)
{
	return vec / vectorLength(vec);
}

inline sf::Vector2f perpendicular(sf::Vector2f vec)
{
	return sf::Vector2f(vec.y, -vec.x);
}

inline sf::Vector2f rotateVector(sf::Vector2f vec, float angle)
{
	float sinv = std::sin(angle), cosv = std::cos(angle);
	return sf::Vector2f(cosv * vec.x - sinv * vec.y, sinv * vec.x + cosv * vec.y);
}

inline float angleBetween(sf::Vector2f a, sf::Vector2f b)
{
	float ret = atan2(a.y-b.y,a.x-b.x);
	return std::isnan(ret) ? 0 : ret;
}

inline float dotProduct(sf::Vector2f a, sf::Vector2f b)
{
	return a.x * b.x + a.y * b.y;
}

inline float distanceToLine(sf::Vector2f a, sf::Vector2f b, sf::Vector2f p)
{
	if (a == b)
		return vectorLength(a - p);

	float lenSq = vectorSquareLength(b-a);						  // squared length of AB.
	sf::Vector2f proj = a + (dotProduct(p-a, b-a) / lenSq) * (b-a); // project point P onto line AB.

	return vectorLength(proj - p);
}

inline bool aboveLine(sf::Vector2f a, sf::Vector2f b, sf::Vector2f p, float off = 0.f)
{
	return (b.x-a.x)*(p.y-a.y) - (b.y-a.y)*(p.x-a.x) + off > 0;
}

inline float clampedDistanceToLine(sf::Vector2f a, sf::Vector2f b, sf::Vector2f p, bool side, float off = 0.f)
{
	return (aboveLine(a, b, p, off) == side) ? distanceToLine(a, b, p) + off : 0;
}

inline float degToRad(float angle)
{
	return angle * (3.14159f / 180.f);
}

inline float radToDeg(float angle)
{
	return angle * (180.f / 3.14159f);
}

inline int iPow(int base, unsigned int exp)
{
	int n = 1;
	for (unsigned int i = 0; i < exp; ++i)
		n *= base;
	return n;
}

inline bool getBarycentricCoords(sf::Vector2f pos, sf::Vector2f t1, sf::Vector2f t2, sf::Vector2f t3, float & b1, float & b2, float & b3)
{
	float dy2y3 = t2.y - t3.y;
	float dx3x2 = t3.x - t2.x;
	float dy1y3 = t1.y - t3.y;
	float dx1x3 = t1.x - t3.x;
	float det = dy2y3 * dx1x3 + dx3x2 * dy1y3;

	if (det == 0)
		return false;

	pos -= t3;
	b1 = (dy2y3 * pos.x + dx3x2 * pos.y) / det;
	b2 = (-dy1y3 * pos.x + dx1x3 * pos.y) / det;
	b3 = 1 - b1 - b2;

	return true;
}

#endif
