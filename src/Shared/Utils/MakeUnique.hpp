#ifndef MAKE_UNIQUE_HPP
#define MAKE_UNIQUE_HPP

#include <memory>

template<typename T, typename... Args>
std::unique_ptr<T> makeUnique(Args&&... args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif
