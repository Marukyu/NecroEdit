#ifndef TIMER_HPP
#define TIMER_HPP

#include <SFML/System/Clock.hpp>

// repeating clock that ticks in a specified interval.
// balances irregular timings.
class Timer {

public:

	Timer();

	bool tick();
	void reset();	   // resets without firing.
	void resetHigh();   // resets and fires immediately.

	void setTime(sf::Time time);
	sf::Time getTime() const;

	float getCurrentRatio() const;

private:

	static sf::Clock clock;
	sf::Time myResetTime, myTime;

};

// repeating clock that sleeps until the next tick.
// if the delay between two ticks is too high, it resets.
class FramerateTimer {

public:

	FramerateTimer();

	void tick();
	void reset();

	void setFrameTime(sf::Time frameTime);
	sf::Time getFrameTime() const;

private:

	static sf::Clock clock;
	sf::Time mySleepTime, myFrameTime;

};

// single clock that counts down for a specified time.
class Countdown {

public:

	Countdown();

	void restart(sf::Time limit);
	sf::Time getRemainingTime() const;
	sf::Time getOvertime() const;
	bool expired() const;

private:

	static sf::Clock clock;
	sf::Time myTimerEnd;

};

// pausable clock that measures time passed.
class Stopwatch {

public:

	Stopwatch();

	void start();
	void stop();
	void reset();

	bool isRunning() const;
	sf::Time getTime() const;

private:

	bool myIsRunning;
	sf::Clock myClock;
	sf::Time myTotalTime;

};

#endif
