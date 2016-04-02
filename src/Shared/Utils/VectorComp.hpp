/*
 * VectorComp.hpp
 *
 *  Created on: Dec 9, 2015
 *      Author: bbq
 */

#ifndef SRC_SHARED_UTILS_VECTORCOMP_HPP_
#define SRC_SHARED_UTILS_VECTORCOMP_HPP_

#include <SFML/System/Vector2.hpp>

namespace sf
{

template <typename T>
inline bool operator<(const Vector2<T>& left, const Vector2<T>& right)
{
	return (left.x < right.x) ? true : (left.x == right.x && left.y < right.y);
}

}

#endif /* SRC_SHARED_UTILS_VECTORCOMP_HPP_ */
