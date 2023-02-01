#ifndef SPINDULYS_FWD_H
#define SPINDULYS_FWD_H

#include "../spindulys.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

template<typename T>
constexpr uint32_t operator|(T f1, T f2)
{
	return (uint32_t) f1 | (uint32_t) f2;
}

template<typename T>
constexpr uint32_t operator|(uint32_t f1, T f2)
{
	return f1 | (uint32_t) f2;
}

template<typename T>
constexpr uint32_t operator&(T f1, T f2)
{
	return (uint32_t) f1 & (uint32_t) f2;
}

template<typename T>
constexpr uint32_t operator&(uint32_t f1, T f2)
{
	return f1 & (uint32_t) f2;
}

template<typename T>
constexpr uint32_t operator~(T f1) { return ~(uint32_t) f1; }

template<typename T>
constexpr uint32_t operator+(T e) { return (uint32_t) e; }

template <typename T>
constexpr bool HasFlag(uint32_t flags, T f)
{
	return (flags & (uint32_t) f) != 0u;
}

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_FWD_H
