#ifndef POLL_QUEUE_HPP
#define POLL_QUEUE_HPP

#include <queue>

template <typename Event>
class PollQueue
{
public:

	void push(Event && event)
	{
		myQueue.push(std::move(event));
	}

	bool poll(Event & event)
	{
		if (myQueue.empty())
		{
			return false;
		}
		else
		{
			event = std::move(myQueue.front());
			myQueue.pop();
			return true;
		}
	}

	void clear()
	{
		std::queue<Event>().swap(myQueue);
	}

	std::size_t size() const
	{
		return myQueue.size();
	}

private:

	std::queue<Event> myQueue;
};

#endif
