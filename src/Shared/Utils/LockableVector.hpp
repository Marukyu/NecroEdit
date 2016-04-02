/*
 * LockableVector.hpp
 *
 *  Created on: Jul 16, 2015
 *      Author: marukyu
 */

#ifndef SRC_SHARED_UTILS_LOCKABLEVECTOR_HPP_
#define SRC_SHARED_UTILS_LOCKABLEVECTOR_HPP_

#include <vector>
#include <limits>
#include <cassert>
#include <iostream>

template<typename T>
class LockableVector : public std::vector<T>
{
public:

	LockableVector() :
		myLockedVector(),
		myHasLockedVector(false),
		myLockedIteratorCount(0)
	{
	}

	class LockingIterator
	{
	public:
		LockingIterator(LockingIterator && other) = default;
		LockingIterator(const LockingIterator & other)
		{
			myParent->incrementLockedIteratorCount();
			myParent = other.myParent;
			myIsReverse = other.myIsReverse;
			myIndex = other.myIndex;
		}

		LockingIterator & operator=(LockingIterator && other) = default;
		LockingIterator & operator=(LockingIterator other)
		{
			std::swap(*this, other);
			return *this;
		}

		~LockingIterator()
		{
			myParent->decrementLockedIteratorCount();
		}

		const T & operator*() const
		{
			return *myParent->getLockedItem((std::size_t) myIndex);
		}

		const T * operator->() const
		{
			return myParent->getLockedItem((std::size_t) myIndex);
		}

		LockingIterator & operator++()
		{
			if (myIsReverse)
				--myIndex;
			else
				++myIndex;
			return *this;
		}

		LockingIterator operator++(int)
		{
			auto copy = *this;
			operator++();
			return copy;
		}

		bool operator==(const LockingIterator & other) const
		{
			return myIndex == other.myIndex;
		}

		bool operator!=(const LockingIterator & other) const
		{
			return myIndex != other.myIndex;
		}

	private:

		LockingIterator(const LockableVector * parent, bool isReverse, std::size_t index)
		{
			myParent = parent;
			myIsReverse = isReverse;
			myIndex = index;
			myParent->incrementLockedIteratorCount();
		}

		const LockableVector * myParent;
		bool myIsReverse;
		std::size_t myIndex;

		friend class LockableVector;
	};

	LockingIterator lbegin() const
	{
		return LockingIterator(this, false, 0);
	}

	LockingIterator lend() const
	{
		return LockingIterator(this, false, std::vector<T>::size());
	}

	LockingIterator lrbegin() const
	{
		return LockingIterator(this, true, std::vector<T>::size() - 1);
	}

	LockingIterator lrend() const
	{
		return LockingIterator(this, true, std::numeric_limits<std::size_t>::max());
	}

	/**
	 * Call this just before making any changes to the vector.
	 */
	void invalidate()
	{
		if (myLockedIteratorCount > 0 && !myHasLockedVector)
		{
			myLockedVector = *this;
		}
	}

private:

	void incrementLockedIteratorCount() const
	{
		myLockedIteratorCount++;
	}

	void decrementLockedIteratorCount() const
	{
		assert(myLockedIteratorCount > 0);

		myLockedIteratorCount--;

		if (myLockedIteratorCount == 0)
		{
			myLockedVector.clear();
			myHasLockedVector = false;
		}
	}

	const T * getLockedItem(std::size_t index) const
	{
		if (myHasLockedVector)
		{
			std::cout << "yes" << std::endl;
			return &myLockedVector[index];
		}
		else
		{
			std::cout << "no" << std::endl;
			return &std::vector<T>::operator[](index);
		}
	}

	mutable std::vector<T> myLockedVector;
	mutable bool myHasLockedVector;
	mutable unsigned int myLockedIteratorCount;

	friend class LockingIterator;
};

#endif
