#ifndef EVENT_MANAGER_HPP
#define EVENT_MANAGER_HPP

#include "Shared/Utils/Event/EventListener.hpp"

template<typename Event>
class EventManager
{
public:

	EventManager() = default;
	EventManager(EventManager && other) = default;

	EventManager & operator=(EventManager && other)
	{
		myQueueList = std::move(other.myQueueList);
		return *this;
	}

	EventManager(const EventManager & other) = delete;
	EventManager & operator=(const EventManager & other) = delete;

	void push(Event event)
	{
		if (!myQueueList.empty())
		{
			std::shared_ptr<Event> eventPtr = std::make_shared<Event>(event);

			for (auto it = myQueueList.begin(); it != myQueueList.end();)
			{
				if (it->expired())
				{
					it = myQueueList.erase(it);
				}
				else
				{
					it->lock()->push(std::move(eventPtr));
					it++;
				}
			}
		}
	}

	EventListener<Event> acquireListener()
	{
		auto newQueue = std::make_shared<std::queue<std::shared_ptr<Event> > >();
		myQueueList.push_back(newQueue);
		return EventListener<Event>(newQueue);
	}

private:

	std::vector<std::weak_ptr<std::queue<std::shared_ptr<Event> > > > myQueueList;
};

#endif
