#include "Shared/Utils/StrError.hpp"

std::string StringError::getErrorString() const
{
	return _myError;
}

int StringError::getErrorId() const
{
	return _myErrorId;
}

void StringError::error(int errId, std::string errStr) const
{
	_myError = errStr;
	_myErrorId = errId;
}
