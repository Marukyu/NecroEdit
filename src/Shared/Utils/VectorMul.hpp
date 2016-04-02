#ifndef VECTOR_MUL_HPP
#define VECTOR_MUL_HPP

#include <SFML/System/Vector2.hpp>

template<typename T>
sf::Vector2<T> operator* (sf::Vector2<T> left, sf::Vector2<T> right)
{
	return sf::Vector2<T>(left.x * right.x, left.y * right.y);
}

template<typename T>
sf::Vector2<T> operator/ (sf::Vector2<T> left, sf::Vector2<T> right)
{
	return sf::Vector2<T>(left.x / right.x, left.y / right.y);
}

template<typename T>
sf::Vector2<T> operator*= (sf::Vector2<T> & left, sf::Vector2<T> right)
{
	return left = left * right;
}

template<typename T>
sf::Vector2<T> operator/= (sf::Vector2<T> & left, sf::Vector2<T> right)
{
	return left = left / right;
}

#endif
