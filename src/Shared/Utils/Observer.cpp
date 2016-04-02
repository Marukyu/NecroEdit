#include "Shared/Utils/Observer.hpp"

Observer::~Observer()
{
	while (!mySubjects.empty())
	{
		(*mySubjects.begin())->removeObserver(this);
	}
}

Observable::~Observable()
{
	while (!myObservers.empty())
	{
		removeObserver(*myObservers.begin());
	}
}

void Observable::addObserver(Observer* observer)
{
	myObservers.insert(observer);
	observer->mySubjects.insert(this);
}

void Observable::removeObserver(Observer* observer)
{
	myObservers.erase(observer);
	observer->mySubjects.erase(this);
}

bool Observable::isObserving(Observer* observer) const
{
	return myObservers.count(observer);
}

void Observable::notify(int message)
{
	for (auto observer : myObservers)
	{
		observer->onNotify(*this, message);
	}
}

