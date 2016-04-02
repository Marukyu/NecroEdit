#ifndef NET_TYPES_HPP
#define NET_TYPES_HPP

#include <SFML/Config.hpp>

class DataStream;

namespace nt
{

template<typename T>
struct VIntWrapper
{
	VIntWrapper(T & value) :
		value(value)
	{}

	T & value;
};

template<typename T>
struct CVIntWrapper
{
	CVIntWrapper(const T & value) :
		value(value)
	{}

	const T & value;
};

inline VIntWrapper<sf::Int64> VInt(sf::Int64 & value)
{
	return VIntWrapper<sf::Int64>(value);
}
inline CVIntWrapper<sf::Int64> VInt(const sf::Int64 & value)
{
	return CVIntWrapper<sf::Int64>(value);
}

inline VIntWrapper<sf::Uint64> VInt(sf::Uint64 & value)
{
	return VIntWrapper<sf::Uint64>(value);
}
inline CVIntWrapper<sf::Uint64> VInt(const sf::Uint64 & value)
{
	return CVIntWrapper<sf::Uint64>(value);
}

inline VIntWrapper<sf::Int32> VInt(sf::Int32 & value)
{
	return VIntWrapper<sf::Int32>(value);
}
inline CVIntWrapper<sf::Int32> VInt(const sf::Int32 & value)
{
	return CVIntWrapper<sf::Int32>(value);
}

inline VIntWrapper<sf::Uint32> VInt(sf::Uint32 & value)
{
	return VIntWrapper<sf::Uint32>(value);
}
inline CVIntWrapper<sf::Uint32> VInt(const sf::Uint32 & value)
{
	return CVIntWrapper<sf::Uint32>(value);
}


DataStream & operator<< (DataStream & stream, const CVIntWrapper<sf::Int64> & data);
DataStream & operator<< (DataStream & stream, const VIntWrapper<sf::Int64> & data);
DataStream & operator>> (DataStream & stream, const VIntWrapper<sf::Int64> & data);

DataStream & operator<< (DataStream & stream, const CVIntWrapper<sf::Uint64> & data);
DataStream & operator<< (DataStream & stream, const VIntWrapper<sf::Uint64> & data);
DataStream & operator>> (DataStream & stream, const VIntWrapper<sf::Uint64> & data);

DataStream & operator<< (DataStream & stream, const CVIntWrapper<sf::Int32> & data);
DataStream & operator<< (DataStream & stream, const VIntWrapper<sf::Int32> & data);
DataStream & operator>> (DataStream & stream, const VIntWrapper<sf::Int32> & data);

DataStream & operator<< (DataStream & stream, const CVIntWrapper<sf::Uint32> & data);
DataStream & operator<< (DataStream & stream, const VIntWrapper<sf::Uint32> & data);
DataStream & operator>> (DataStream & stream, const VIntWrapper<sf::Uint32> & data);

}

#endif
