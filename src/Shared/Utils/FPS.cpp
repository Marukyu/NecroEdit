#include "Shared/Utils/FPS.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

FpsCounter::FpsCounter()
{
	myFPS = 0.f;
	setSampleSize(70);
	myClock.restart();
}

void FpsCounter::setSampleSize(std::size_t size)
{
	myTimeVecMaxSize = std::max<std::size_t>(size, 1);
	myTimes.reserve(size);
	reset();
}

std::size_t FpsCounter::getSampleSize() const
{
	return myTimeVecMaxSize;
}

void FpsCounter::tick()
{
	const sf::Time elapsedTime = myClock.restart();

	if (myTimeVecOffset >= myTimeVecMaxSize)
	{
		myTimeVecOffset = 0;
	}

	sf::Time prevTime = sf::Time::Zero;

	if (myTimes.size() <= myTimeVecOffset)
	{
		myTimes.resize(myTimeVecOffset+1);
	}
	else
	{
		prevTime = myTimes[myTimeVecOffset];
	}

	myTimes[myTimeVecOffset] = elapsedTime;

	myTotalTime += elapsedTime;
	myTotalTime -= prevTime;

	//if (myTimeVecOffset == 0)
	{
		myFPS = 1000000.0 / (myTotalTime.asMicroseconds() / myTimes.size());
	}

	++myTimeVecOffset;

	/*
	my_tick--;
	if (my_tick == 0)
	{
		const sf::Time elapsed_time = myClock.getElapsedTime();

		if (elapsed_time != sf::Time::Zero)
			fps = 1000000.0 / ((float)elapsed_time.asMicroseconds() / (float)max_tick);

		reset();
	}
	*/
}

void FpsCounter::reset()
{
	myTimes.clear();
	myTimeVecOffset = 0;
	myTotalTime = sf::Time::Zero;
}

float FpsCounter::getFPS() const
{
	return myFPS;
}

unsigned int FpsCounter::getFPSi() const
{
	return std::floor(myFPS+0.5);
}
