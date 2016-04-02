#ifndef SPACE_CONTAINER_HPP
#define SPACE_CONTAINER_HPP

#include <vector>

typedef std::size_t spaceEntry;

template <typename MyType>
class SpaceContainer {
public:

	// default constructor: creates empty container.
	SpaceContainer()
	{
		myEntryCount = 0;
	}

	// copy assignment.
	SpaceContainer<MyType> & operator=(const SpaceContainer<MyType> & orig)
	{
		clear();
		for (spaceEntry i = orig.first(); i < orig.end(); orig.next(i))
		{
			addAt(i, new MyType(orig[i]));
		}
		return *this;
	}

	// move assignment.
	SpaceContainer<MyType> & operator=(SpaceContainer<MyType> && orig)
	{
		content = std::move(orig.content);
		myEntryCount = std::move(orig.myEntryCount);
		return *this;
	}

	// copy constructor.
	SpaceContainer<MyType>(const SpaceContainer<MyType> & orig)
	{
		*this = orig;
	}

	// move constructor.
	SpaceContainer<MyType>(SpaceContainer<MyType> && orig)
	{
		content = std::move(orig.content);
		myEntryCount = std::move(orig.myEntryCount);
	}

	// removes the specified entry.
	void remove(spaceEntry index)
	{
		if (index >= content.size()) return;
		if (content[index] == 0) return;

		myEntryCount--;

		delete content[index];
		content[index] = 0;
		for (spaceEntry i = content.size()-1; content[i] == 0; --i)
		{
			content.resize(content.size()-1);
			if (i == 0) break;
		}
	}

	// removes the last entry.
	void remove()
	{
		if (getSize() != 0)
		{
			remove(end() - 1);
		}
	}

	// removes all entries.
	void clear()
	{
		for (spaceEntry i = 0; i < content.size(); ++i)
			if (content[i] != 0)
			{
				delete content[i];
				content[i] = 0;
			}
		content.clear();
		myEntryCount = 0;
	}

	// destructor.
	~SpaceContainer()
	{
		clear();
	}

	// returns the first free entry.
	spaceEntry getFirstFree(spaceEntry skip = 0) const
	{
		for (spaceEntry i = skip; i < content.size(); ++i)
		{
			if (content[i] == 0)
			{
				return i;
			}
		}
		return std::max(skip, content.size());
	}

	// adds an entry at the specified position.
	// will *not* overwrite entry if it already exists.
	spaceEntry addAt(spaceEntry pos, MyType * ptr)
	{
		if (has(pos))
			return pos;

		myEntryCount++;

		if (pos >= content.size())
			content.resize(pos+1);

		content[pos] = ptr;
		return pos;
	}

	// adds an entry at the specified position.
	// will *not* overwrite entry if it already exists.
	spaceEntry addAt(spaceEntry pos)
	{
		return addAt(pos, new MyType);
	}

	// adds an entry at the first free slot.
	spaceEntry add(MyType * ptr)
	{
		return addAt(getFirstFree(), ptr);
	}

	// adds an entry at the first free slot.
	spaceEntry add()
	{
		return add(new MyType);
	}

	// returns the specified entry by reference. unsafe.
	MyType & operator[](spaceEntry index)
	{
		return *content[index];
	}

	// returns the specified entry by const reference. unsafe.
	const MyType & operator[](spaceEntry index) const
	{
		return *content[index];
	}

	// returns the total number of entries.
	std::size_t getSize() const
	{
		/*
		std::size_t ret = 0;
		for (spaceEntry i = 0; i < content.size(); ++i)
			if (content[i] != 0) ++ret;
		return ret;
		*/
		return myEntryCount;
	}

	// returns one past the last slot of the container.
	spaceEntry end() const
	{
		return content.size();
	}

	// increments a container iterator.
	void next(spaceEntry & cur) const
	{
		spaceEntry i = cur+1;
		while (true)
		{
			if (i >= content.size())
			{
				cur = end();
				return;
			}
			if (content[i] != 0)
			{
				cur = i;
				return;
			}
			++i;
		}
	}

	// returns an iterator to the first element in the container.
	spaceEntry first() const
	{
		spaceEntry i = 0;
		while (true)
		{
			if (i >= content.size())
			{
				return end();
			}
			if (content[i] != 0)
			{
				return i;
			}
			++i;
		}
	}

	// returns true if the specified entry exists in the container.
	bool has(spaceEntry index) const
	{
		return index < content.size() && content[index] != 0;
	}

private:
	std::vector<MyType *> content;
	std::size_t myEntryCount;

};

#endif
