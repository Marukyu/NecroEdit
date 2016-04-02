#ifndef EVENT_LISTENER_HPP
#define EVENT_LISTENER_HPP

#include <memory>
#include <queue>

/// a class that allows for shared listening for events.
template <typename Event>
class EventListener
{
public:

	EventListener() = default;
	EventListener(EventListener && other) = default;

	EventListener & operator=(EventListener && other)
	{
		myEventQueue = std::move(other.myEventQueue);
		return *this;
	}

	EventListener(const EventListener & other) = delete;
	EventListener & operator=(const EventListener & other) = delete;

	bool poll(Event & event)
	{
		if (!myEventQueue || myEventQueue->empty())
		{
			return false;
		}
		else
		{
			event = *myEventQueue->front();
			myEventQueue->pop();
			return true;
		}
	}

private:

	EventListener(std::shared_ptr<std::queue<std::shared_ptr<Event> > > queue)
	{
		myEventQueue = queue;
	}

	std::shared_ptr<std::queue<std::shared_ptr<Event> > > myEventQueue;

	template<typename>
	friend class EventManager;
};

#endif
