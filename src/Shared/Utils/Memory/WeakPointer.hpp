/*
 * WeakPointer.hpp
 *
 *  Created on: Mar 2, 2015
 *      Author: marukyu
 */

#ifndef SRC_SHARED_UTILS_WEAKPOINTER_HPP_
#define SRC_SHARED_UTILS_WEAKPOINTER_HPP_

#include <boost/container/flat_set.hpp>

template<typename T>
class WeakPointerHolder;

template<typename T>
class WeakPointer
{
public:

	using TargetType = T;

	WeakPointer();
	WeakPointer(nullptr_t null);

	WeakPointer(WeakPointer<T> && ptr);
	WeakPointer(const WeakPointer<T> & ptr);
	WeakPointer<T> & operator=(WeakPointer<T> && ptr);
	WeakPointer<T> & operator=(const WeakPointer<T> & ptr);

	virtual ~WeakPointer();

	bool isExpired() const;

	T * get() const;
	T & operator*() const;
	T * operator->() const;

	template<typename NewType>
	WeakPointer<NewType> dynamicCast() const;

private:

	WeakPointer(WeakPointerHolder<T> * target);

	void expire();

	void changeTarget(WeakPointerHolder<T> * parent);

	void registerSelf();
	void unregisterSelf();

	WeakPointerHolder<T> * myTarget;

	friend class WeakPointerHolder<T> ;
};

template<typename T>
class WeakPointerHolder
{

public:

	/**
	 * Default constructor. Initializes a weak pointer holder without any weak pointers.
	 */
	WeakPointerHolder()
	{
	}

	/**
	 * Move constructor. Passes all weak pointers to the new holder.
	 */
	WeakPointerHolder(WeakPointerHolder<T> && holder)
	{
		myPointers = std::move(holder.myPointers);

		for (WeakPointer<T> * ptr : myPointers)
		{
			ptr->changeParent(this);
		}
	}

	/**
	 * Move assignment. Passes all weak pointers to the left side object of the assignment.
	 */
	WeakPointerHolder<T> & operator=(WeakPointerHolder<T> && holder)
	{
		myPointers = std::move(holder.myPointers);

		for (WeakPointer<T> * ptr : myPointers)
		{
			ptr->changeTarget(this);
		}
		return *this;
	}

	/**
	 * Destructor. Expires all weak pointers pointing at this holder.
	 */
	virtual ~WeakPointerHolder()
	{
		for (WeakPointer<T> * ptr : myPointers)
		{
			ptr->expire();
		}
	}

	/**
	 * Returns a weak pointer to this holder that will expire when the holder gets destroyed.
	 */
	WeakPointer<T> getWeakPointer() const
	{
		return WeakPointer<T>(const_cast<WeakPointerHolder*>(this));
	}

private:

	WeakPointerHolder(WeakPointerHolder<T> & holder)
	{
	}

	WeakPointerHolder<T> & operator=(WeakPointerHolder<T> & holder)
	{
		return *this;
	}

	void registerWeakPointer(WeakPointer<T> * ptr)
	{
		myPointers.insert(ptr);
	}

	void unregisterWeakPointer(WeakPointer<T> * ptr)
	{
		myPointers.erase(ptr);
	}

	mutable boost::container::flat_set<WeakPointer<T>*> myPointers;

	friend class WeakPointer<T> ;
};

template<typename T>
WeakPointer<T>::WeakPointer() :
		myTarget(nullptr)
{
}

template<typename T>
WeakPointer<T>::WeakPointer(nullptr_t null) :
		myTarget(nullptr)
{
}

template<typename T>
WeakPointer<T>::WeakPointer(WeakPointer<T> && ptr)
{
	ptr.unregisterSelf();
	myTarget = std::move(ptr.myTarget);
	registerSelf();
}

template<typename T>
WeakPointer<T>::WeakPointer(const WeakPointer<T> & ptr)
{
	myTarget = ptr.myTarget;
	registerSelf();
}
template<typename T>
WeakPointer<T> &
WeakPointer<T>::operator=(WeakPointer<T> && ptr)
{
	ptr.unregisterSelf();
	myTarget = std::move(ptr.myTarget);
	registerSelf();
	return *this;
}

template<typename T>
WeakPointer<T> & WeakPointer<T>::operator=(const WeakPointer<T> & ptr)
{
	myTarget = ptr.myTarget;
	registerSelf();
	return *this;
}

template<typename T>
WeakPointer<T>::~WeakPointer()
{
	unregisterSelf();
}

template<typename T>
bool WeakPointer<T>::isExpired() const
{
	return myTarget == nullptr;
}

template<typename T>
T * WeakPointer<T>::get() const
{
	return dynamic_cast<T*>(myTarget);
}

template<typename T>
T & WeakPointer<T>::operator*() const
{
	return dynamic_cast<T&>(*myTarget);
}

template<typename T>
T * WeakPointer<T>::operator->() const
{
	return get();
}

template<typename T> template<typename NewType>
WeakPointer<NewType> WeakPointer<T>::dynamicCast() const
{
	return WeakPointer<NewType>(dynamic_cast<NewType*>(myTarget));
}

template<typename T>
WeakPointer<T>::WeakPointer(WeakPointerHolder<T> * target) :
		myTarget(target)
{
	registerSelf();
}

template<typename T>
void WeakPointer<T>::expire()
{
	myTarget = nullptr;
}

template<typename T>
void WeakPointer<T>::changeTarget(WeakPointerHolder<T> * parent)
{
	myTarget = parent;
}

template<typename T>
void WeakPointer<T>::registerSelf()
{
	if (myTarget != nullptr)
	{
		myTarget->registerWeakPointer(this);
	}
}

template<typename T>
void WeakPointer<T>::unregisterSelf()
{
	if (myTarget != nullptr)
	{
		myTarget->unregisterWeakPointer(this);
	}
}

#endif
