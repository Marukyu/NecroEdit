#ifndef STR_ERROR_HPP
#define STR_ERROR_HPP

#include <string>

class StringError {

public:

	std::string getErrorString() const;
	int getErrorId() const;

protected:

	void error(int errId, std::string errStr) const;

private:

	mutable std::string _myError;
	mutable int _myErrorId;

};

#endif

