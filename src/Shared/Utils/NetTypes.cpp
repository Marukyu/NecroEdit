#include "Shared/Utils/NetTypes.hpp"
#include "Shared/Utils/DataStream.hpp"

namespace nt
{

namespace priv
{
	// returns the unsigned representation of the specified number.
	sf::Uint64 mapSigned(sf::Int64 num)
	{
		if (num < 0)
		{
			// negative numbers are mapped to the odd numbers.
			return sf::Uint64(-num) * 2 - 1;
		}
		else
		{
			// positive numbers and zero are mapped to the even numbers.
			return sf::Uint64(num) * 2;
		}
	}

	// returns the signed number from its unsigned representation.
	sf::Int64 unmapSigned(sf::Uint64 num)
	{
		if (num % 2 == 0)
		{
			// even numbers are mapped back to the positive numbers.
			return sf::Int64(num / 2);
		}
		else
		{
			// odd numbers are mapped back to the negative numbers.
			return -sf::Int64((num - 1) / 2) - 1;
		}
	}

	// gets the byte length of an unsigned VInt.
	std::size_t getVLength(sf::Uint64 num)
	{
		// length of the VInt in bytes.
		std::size_t length = 0;

		// unconditionally perform first shift. even zero needs 1 byte.
		do
		{
			// divide number by 128, or shift right by 7 bits.
			num >>= 7;

			// increase length by 1 byte.
			length++;
		}
		while (num != 0);

		// returns the VInt's length.
		return length;
	}

	// returns the next byte of a VInt.
	sf::Uint8 nextVByte(sf::Uint64 & num)
	{
		// get lowest 7 bits of number.
		sf::Uint8 byte = num & 0x7F;

		// reduce number.
		num >>= 7;

		// add continuation bit.
		if (num != 0)
		{
			byte |= 0x80;
		}

		return byte;
	}

	// adds data to a number from its VInt representation. returns true if VInt continues.
	bool attachVByte(sf::Uint64 & num, sf::Uint8 byte, std::size_t byteIndex)
	{
		// attach lowest 7 bits of the input byte to the number.
		num |= sf::Uint64(byte & 0x7F) << sf::Uint64(byteIndex * 7);

		// return true if there is a continuation bit.
		return byte & 0x80;
	}

	// maximum possible number of bytes a 64-bit VInt can have.
	std::size_t maxVIntLength = 10;


	// adds an unsigned VInt to a data stream.
	void appendVInt(DataStream & stream, sf::Uint64 num)
	{
		std::size_t len = getVLength(num);

		for (std::size_t i = 0; i < len; ++i)
		{
			stream << nextVByte(num);
		}
	}

	// extracts an unsigned VInt from a data stream.
	sf::Uint64 extractVInt(DataStream & stream)
	{
		sf::Uint64 num = 0;

		for (std::size_t i = 0; i < maxVIntLength; ++i)
		{
			sf::Uint8 byte;

			stream >> byte;

			if (!attachVByte(num, byte, i))
			{
				break;
			}
		}

		return num;
	}
}


DataStream & operator<<(DataStream& stream, const CVIntWrapper<sf::Int64>& data)
{
	priv::appendVInt(stream, priv::mapSigned(data.value));
	return stream;
}

DataStream & operator<<(DataStream& stream, const VIntWrapper<sf::Int64>& data)
{
	priv::appendVInt(stream, priv::mapSigned(data.value));
	return stream;
}

DataStream & operator>>(DataStream& stream, const VIntWrapper<sf::Int64>& data)
{
	data.value = priv::unmapSigned(priv::extractVInt(stream));
	return stream;
}


DataStream & operator<<(DataStream& stream, const CVIntWrapper<sf::Uint64>& data)
{
	priv::appendVInt(stream, data.value);
	return stream;
}

DataStream & operator<<(DataStream& stream, const VIntWrapper<sf::Uint64>& data)
{
	priv::appendVInt(stream, data.value);
	return stream;
}

DataStream & operator>>(DataStream& stream, const VIntWrapper<sf::Uint64>& data)
{
	data.value = priv::extractVInt(stream);
	return stream;
}


DataStream & operator<<(DataStream& stream, const CVIntWrapper<sf::Int32>& data)
{
	priv::appendVInt(stream, priv::mapSigned(data.value));
	return stream;
}

DataStream & operator<<(DataStream& stream, const VIntWrapper<sf::Int32>& data)
{
	priv::appendVInt(stream, priv::mapSigned(data.value));
	return stream;
}

DataStream & operator>>(DataStream& stream, const VIntWrapper<sf::Int32>& data)
{
	data.value = priv::unmapSigned(priv::extractVInt(stream));
	return stream;
}


DataStream & operator<<(DataStream& stream, const CVIntWrapper<sf::Uint32>& data)
{
	priv::appendVInt(stream, data.value);
	return stream;
}

DataStream & operator<<(DataStream& stream, const VIntWrapper<sf::Uint32>& data)
{
	priv::appendVInt(stream, data.value);
	return stream;
}

DataStream & operator>>(DataStream& stream, const VIntWrapper<sf::Uint32>& data)
{
	data.value = priv::extractVInt(stream);
	return stream;
}




}


