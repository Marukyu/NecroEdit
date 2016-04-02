#include "Shared/Utils/Timer.hpp"

#include <SFML/System/Sleep.hpp>
#include <algorithm>


sf::Clock Timer::clock = sf::Clock();

Timer::Timer()
{
	setTime(sf::milliseconds(1000));
}

bool Timer::tick()
{
	if (clock.getElapsedTime() - myResetTime >= myTime)
	{	   // timer ticked.
		myResetTime += myTime;
		return true;
	}
	else	// timer didn't tick.
		return false;
}

void Timer::reset()
{
	myResetTime = clock.getElapsedTime();
}

void Timer::resetHigh()
{
	myResetTime = clock.getElapsedTime() - myTime;
}

void Timer::setTime(sf::Time time)
{
	myTime = time;
	reset();
}

sf::Time Timer::getTime() const
{
	return myTime;
}

float Timer::getCurrentRatio() const
{
	return (clock.getElapsedTime() - myResetTime).asSeconds() / myTime.asSeconds();
}


sf::Clock FramerateTimer::clock = sf::Clock();

FramerateTimer::FramerateTimer()
{
	setFrameTime(sf::microseconds(1000000 / 60));
}

void FramerateTimer::tick()
{
	if (-clock.getElapsedTime() + mySleepTime < sf::Time::Zero)
	{
		reset();
	}
	else
	{
		sf::sleep(-clock.getElapsedTime() + mySleepTime);
		mySleepTime += myFrameTime;
	}
}

void FramerateTimer::reset()
{
	mySleepTime = clock.getElapsedTime() + myFrameTime;
}

void FramerateTimer::setFrameTime(sf::Time frameTime)
{
	myFrameTime = frameTime;
	reset();
}

sf::Time FramerateTimer::getFrameTime() const
{
	return myFrameTime;
}



sf::Clock Countdown::clock = sf::Clock();

Countdown::Countdown()
{
	restart(sf::Time::Zero);
}

void Countdown::restart(sf::Time limit)
{
	myTimerEnd = clock.getElapsedTime() + limit;
}

sf::Time Countdown::getRemainingTime() const
{
	return std::max(myTimerEnd - clock.getElapsedTime(), sf::Time::Zero);
}

sf::Time Countdown::getOvertime() const
{
	return clock.getElapsedTime() - myTimerEnd;
}

bool Countdown::expired() const
{
	return clock.getElapsedTime() >= myTimerEnd;
}



Stopwatch::Stopwatch()
{
	reset();
}

void Stopwatch::start()
{
	if (isRunning())
		return;

	myIsRunning = true;
	myClock.restart();
}

void Stopwatch::stop()
{
	if (!isRunning())
		return;

	myTotalTime += myClock.getElapsedTime();
	myIsRunning = false;
}

void Stopwatch::reset()
{
	myTotalTime = sf::Time::Zero;
	myIsRunning = false;
}

bool Stopwatch::isRunning() const
{
	return myIsRunning;
}

sf::Time Stopwatch::getTime() const
{
	if (isRunning())
		return myTotalTime + myClock.getElapsedTime();
	else
		return myTotalTime;
}
