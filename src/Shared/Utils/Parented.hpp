#ifndef PARENTED_HPP
#define PARENTED_HPP

#include <exception>

class ParentMissingException : public std::exception {

public:

	virtual const char* what() const throw ()
	{
		return "Missing parent";
	}

};

template<typename ParentType>
class Parented {

public:

	Parented()
	{
		myParent = 0;
	}

	virtual ~Parented()
	{

	}

	void setParent(ParentType * parent)
	{
		if (myParent != parent)
		{
			ParentType * prevParent = myParent;
			myParent = parent;
			onParent(prevParent);
		}
	}

	bool hasParent() const
	{
		return myParent != 0;
	}

	ParentType & getParent() const throw(ParentMissingException)
	{
		if (!hasParent())
		{
			throw (ParentMissingException());
		}

		return *myParent;
	}

private:

	virtual void onParent(ParentType * oldParent) {}

	ParentType * myParent;

};

#endif
