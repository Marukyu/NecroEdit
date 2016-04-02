/*
 * CallbackManager.hpp
 *
 *  Created on: Mar 7, 2015
 *      Author: marukyu
 */

#ifndef SRC_SHARED_UTILS_EVENT_CALLBACKMANAGER_HPP_
#define SRC_SHARED_UTILS_EVENT_CALLBACKMANAGER_HPP_

#include <algorithm>
#include <functional>
#include <vector>

template<typename ... Args>
class CallbackManager
{
public:

	using Func = std::function<void(Args...)>;

	typedef unsigned int ID;

	CallbackManager() :
			myIDCounter(0)
	{
	}

	ID addCallback(Func function, int filter, int priority)
	{
		CallbackStruct callback;

		callback.function = std::move(function);
		callback.filter = std::move(filter);
		callback.priority = std::move(priority);
		callback.id = ++myIDCounter;

		auto insertPos = std::upper_bound(myCallbacks.begin(), myCallbacks.end(), callback);
		myCallbacks.insert(insertPos, std::move(callback));

		return myIDCounter;
	}

	void fireCallback(int filter, Args ... args) const
	{
		for (const auto & callback : myCallbacks)
		{
			if (callback.filter & filter)
			{
				callback.function(args...);
			}
		}
	}

	void removeCallback(ID id)
	{
		auto foundPos = std::lower_bound(myCallbacks.begin(), myCallbacks.end(), id,
			[](const CallbackStruct & cb1, const CallbackStruct & cb2)
			{
				return cb1.id < cb2.id;
			});
		
		if (foundPos != myCallbacks.end() && *foundPos == id)
		{
			myCallbacks.erase(foundPos);
		}
	}

	void clearCallbacks()
	{
		myCallbacks.clear();
	}

private:

	struct CallbackStruct
	{
		Func function;
		int filter;
		int priority;
		ID id;

		bool operator<(const CallbackStruct & other) const
		{
			return priority > other.priority;
		}
	};

	ID myIDCounter;

	std::vector<CallbackStruct> myCallbacks;
};

#endif
