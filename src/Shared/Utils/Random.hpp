#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <SFML/Config.hpp>

class Random
{
public:

	static sf::Uint8 generate8();
	static sf::Uint16 generate16();
	static sf::Uint32 generate32();
	static sf::Uint64 generate64();
	static float generateNormFloat();
};

#endif
