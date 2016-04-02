#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <set>

class Observable;

class Observer
{
public:

	Observer() = default;
	Observer(const Observer & observer) = delete;
	virtual ~Observer();

private:
	virtual void onNotify(Observable & subject, int message) = 0;

	std::set<Observable*> mySubjects;

	friend class Observable;
};

class Observable
{
public:

	Observable() = default;
	Observable(const Observable & observable) = delete;
	virtual ~Observable();

	void addObserver(Observer * observer);
	void removeObserver(Observer * observer);
	bool isObserving(Observer * observer) const;

protected:

	void notify(int message = 0);

private:

	std::set<Observer*> myObservers;
};

#endif
