#ifndef FPS_HPP
#define FPS_HPP

#include <SFML/System/Clock.hpp>
#include <vector>

class FpsCounter {

public:
	FpsCounter();

	void tick();
	void reset();

	float getFPS() const;
	unsigned int getFPSi() const;

	void setSampleSize(std::size_t size);
	std::size_t getSampleSize() const;

private:

	sf::Clock myClock;
	std::size_t myTimeVecOffset, myTimeVecMaxSize;
	std::vector<sf::Time> myTimes;
	sf::Time myTotalTime;
	float myFPS;
};

#endif
