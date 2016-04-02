#include "Shared/Utils/Random.hpp"
#include <random>

namespace randomPriv
{

std::mt19937_64 generator;

std::uniform_int_distribution<sf::Uint16>  dis8 = std::uniform_int_distribution<sf::Uint16>(0, 255);
std::uniform_int_distribution<sf::Uint16> dis16 = std::uniform_int_distribution<sf::Uint16>();
std::uniform_int_distribution<sf::Uint32> dis32 = std::uniform_int_distribution<sf::Uint32>();
std::uniform_int_distribution<sf::Uint64> dis64 = std::uniform_int_distribution<sf::Uint64>();
std::uniform_real_distribution<float>	 disF  = std::uniform_real_distribution<float>();

bool initialized = false;

void init()
{
	if (initialized)
		return;

	std::random_device randomDevice;
	generator.seed(randomDevice());
}

}

sf::Uint8 Random::generate8()
{
	randomPriv::init();
	return randomPriv::dis8(randomPriv::generator);
}

sf::Uint16 Random::generate16()
{
	randomPriv::init();
	return randomPriv::dis16(randomPriv::generator);
}

sf::Uint32 Random::generate32()
{
	randomPriv::init();
	return randomPriv::dis32(randomPriv::generator);
}

sf::Uint64 Random::generate64()
{
	randomPriv::init();
	return randomPriv::dis64(randomPriv::generator);
}

float Random::generateNormFloat()
{
	randomPriv::init();
	return randomPriv::disF(randomPriv::generator);
}

