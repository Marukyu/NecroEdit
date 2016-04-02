#ifndef ENDIAN_HPP
#define ENDIAN_HPP

#include <stdint.h>

#define WOS_NET_LITTLE_ENDIAN

#if	 __BYTE_ORDER == __LITTLE_ENDIAN || _BYTE_ORDER == _LITTLE_ENDIAN	|| BYTE_ORDER == LITTLE_ENDIAN || defined(__LITTLE_ENDIAN__)
#define WOS_LITTLE_ENDIAN
#elif   __BYTE_ORDER == __BIG_ENDIAN	|| _BYTE_ORDER == _BIG_ENDIAN	   || BYTE_ORDER == BIG_ENDIAN	|| defined(__BIG_ENDIAN__)
#define WOS_BIG_ENDIAN
#else
#error "Byte order could not be determined."
#endif

#if (defined(WOS_NET_LITTLE_ENDIAN) && defined(WOS_BIG_ENDIAN)) || (defined(WOS_NET_BIG_ENDIAN) && defined(WOS_LITTLE_ENDIAN))
#define WOS_BYTE_ORDER_SWAP
#endif

inline uint32_t h2nl(uint32_t x)
{
#ifdef WOS_BYTE_ORDER_SWAP
	return  (x>>24) |
			((x<<8) & 0x00FF0000) |
			((x>>8) & 0x0000FF00) |
			(x<<24);
#else
	return x;
#endif
}

inline uint32_t n2hl(uint32_t x)
{
	return h2nl(x);
}

inline void h2nla(uint32_t * x, std::size_t count)
{
#ifdef WOS_BYTE_ORDER_SWAP
	while (count--)
	{
		*x = h2nl(*x);
		++x;
	}
#endif
}

inline void n2hla(uint32_t * x, std::size_t count)
{
	h2nla(x, count);
}


inline uint64_t h2nll(uint64_t x)
{
#ifdef WOS_BYTE_ORDER_SWAP
	return  (x>>56) |
			((x<<40) & 0x00FF000000000000) |
			((x<<24) & 0x0000FF0000000000) |
			((x<<8)  & 0x000000FF00000000) |
			((x>>8)  & 0x00000000FF000000) |
			((x>>24) & 0x0000000000FF0000) |
			((x>>40) & 0x000000000000FF00) |
			(x<<56);
#else
	return x;
#endif
}

inline uint64_t n2hll(uint64_t x)
{
	return h2nll(x);
}

inline void h2nlla(uint64_t * x, std::size_t count)
{
#ifdef WOS_BYTE_ORDER_SWAP
	while (count--)
	{
		*x = h2nll(*x);
		++x;
	}
#endif
}

inline void n2hlla(uint64_t * x, std::size_t count)
{
	h2nlla(x, count);
}


inline uint16_t h2ns(uint16_t x)
{
#ifdef WOS_BYTE_ORDER_SWAP
	return  (x>>8) |
			(x<<8);
#else
	return x;
#endif
}

inline uint16_t n2hs(uint16_t x)
{
	return h2ns(x);
}

inline void h2nsa(uint16_t * x, std::size_t count)
{
#ifdef WOS_BYTE_ORDER_SWAP
	while (count--)
	{
		*x = h2ns(*x);
		++x;
	}
#endif
}

inline void n2hsa(uint16_t * x, std::size_t count)
{
	h2nsa(x, count);
}

#endif
