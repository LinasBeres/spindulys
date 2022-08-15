#ifndef SPINDULYS_PLATFORM_H
#define SPINDULYS_PLATFORM_H

#include "../spindulys.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

#ifdef __WIN32__
#if !defined(__noinline)
#define __noinline             __declspec(noinline)
#endif
#define __forceinline          inline
//#define __restrict           __restrict
#if defined(__INTEL_COMPILER)
#define __restrict__           __restrict
#else
#define __restrict__           //__restrict // causes issues with MSVC
#endif
#if !defined(__thread)
#define __thread               __declspec(thread)
#endif
#if !defined(__aligned)
#define __aligned(...)           __declspec(align(__VA_ARGS__))
#endif
//#define __FUNCTION__           __FUNCTION__
#define debugbreak()           __debugbreak()

#else
#if !defined(__noinline)
#define __noinline             __attribute__((noinline))
#endif
#if !defined(__forceinline)
#define __forceinline          inline __attribute__((always_inline))
#endif
//#define __restrict             __restrict
//#define __thread               __thread
#if !defined(__aligned)
#define __aligned(...)           __attribute__((aligned(__VA_ARGS__)))
#endif
#if !defined(__FUNCTION__)
#define __FUNCTION__           __PRETTY_FUNCTION__
#endif
#define debugbreak()           asm ("int $3")
#endif

#if defined(__clang__) || defined(__GNUC__)
  #define MAYBE_UNUSED __attribute__((unused))
#else
  #define MAYBE_UNUSED
#endif

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_PLATFORM_H
