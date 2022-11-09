#ifndef SPINDULYS_CONSTANTS_H
#define SPINDULYS_CONSTANTS_H

#include "../../spindulys.h"
#include "../platform.h"

#include <limits>

#include <math.h> // using cmath causes issues under Windows
#include <cfloat>

SPINDULYS_NAMESPACE_OPEN_SCOPE

static MAYBE_UNUSED constexpr float one_over_255 = 1.f/255.f;
static MAYBE_UNUSED constexpr float min_rcp_input = 1E-18f;  // for abs(x) >= min_rcp_input the newton raphson rcp calculation does not fail

/* we consider floating point numbers in that range as valid input numbers */
static MAYBE_UNUSED float FLT_LARGE = 1.844E18f;

struct TrueTy {
	__forceinline operator bool( ) const { return true; }
};

extern MAYBE_UNUSED TrueTy True;

struct FalseTy {
	__forceinline operator bool( ) const { return false; }
};

extern MAYBE_UNUSED FalseTy False;

struct ZeroTy
{
	__forceinline operator          double   ( ) const { return 0; }
	__forceinline operator          float    ( ) const { return 0; }
	__forceinline operator          long long( ) const { return 0; }
	__forceinline operator unsigned long long( ) const { return 0; }
	__forceinline operator          long     ( ) const { return 0; }
	__forceinline operator unsigned long     ( ) const { return 0; }
	__forceinline operator          int      ( ) const { return 0; }
	__forceinline operator unsigned int      ( ) const { return 0; }
	__forceinline operator          short    ( ) const { return 0; }
	__forceinline operator unsigned short    ( ) const { return 0; }
	__forceinline operator          char     ( ) const { return 0; }
	__forceinline operator unsigned char     ( ) const { return 0; }
};

extern MAYBE_UNUSED ZeroTy zero;

struct OneTy
{
	__forceinline operator          double   ( ) const { return 1; }
	__forceinline operator          float    ( ) const { return 1; }
	__forceinline operator          long long( ) const { return 1; }
	__forceinline operator unsigned long long( ) const { return 1; }
	__forceinline operator          long     ( ) const { return 1; }
	__forceinline operator unsigned long     ( ) const { return 1; }
	__forceinline operator          int      ( ) const { return 1; }
	__forceinline operator unsigned int      ( ) const { return 1; }
	__forceinline operator          short    ( ) const { return 1; }
	__forceinline operator unsigned short    ( ) const { return 1; }
	__forceinline operator          char     ( ) const { return 1; }
	__forceinline operator unsigned char     ( ) const { return 1; }
};

extern MAYBE_UNUSED OneTy one;

struct NegInfTy
{
	__forceinline operator          double   ( ) const { return -std::numeric_limits<double>::infinity(); }
	__forceinline operator          float    ( ) const { return -std::numeric_limits<float>::infinity(); }
	__forceinline operator          long long( ) const { return std::numeric_limits<long long>::min(); }
	__forceinline operator unsigned long long( ) const { return std::numeric_limits<unsigned long long>::min(); }
	__forceinline operator          long     ( ) const { return std::numeric_limits<long>::min(); }
	__forceinline operator unsigned long     ( ) const { return std::numeric_limits<unsigned long>::min(); }
	__forceinline operator          int      ( ) const { return std::numeric_limits<int>::min(); }
	__forceinline operator unsigned int      ( ) const { return std::numeric_limits<unsigned int>::min(); }
	__forceinline operator          short    ( ) const { return std::numeric_limits<short>::min(); }
	__forceinline operator unsigned short    ( ) const { return std::numeric_limits<unsigned short>::min(); }
	__forceinline operator          char     ( ) const { return std::numeric_limits<char>::min(); }
	__forceinline operator unsigned char     ( ) const { return std::numeric_limits<unsigned char>::min(); }

};

extern MAYBE_UNUSED NegInfTy neg_inf;

struct PosInfTy
{
	__forceinline operator          double   ( ) const { return std::numeric_limits<double>::infinity(); }
	__forceinline operator          float    ( ) const { return std::numeric_limits<float>::infinity(); }
	__forceinline operator          long long( ) const { return std::numeric_limits<long long>::max(); }
	__forceinline operator unsigned long long( ) const { return std::numeric_limits<unsigned long long>::max(); }
	__forceinline operator          long     ( ) const { return std::numeric_limits<long>::max(); }
	__forceinline operator unsigned long     ( ) const { return std::numeric_limits<unsigned long>::max(); }
	__forceinline operator          int      ( ) const { return std::numeric_limits<int>::max(); }
	__forceinline operator unsigned int      ( ) const { return std::numeric_limits<unsigned int>::max(); }
	__forceinline operator          short    ( ) const { return std::numeric_limits<short>::max(); }
	__forceinline operator unsigned short    ( ) const { return std::numeric_limits<unsigned short>::max(); }
	__forceinline operator          char     ( ) const { return std::numeric_limits<char>::max(); }
	__forceinline operator unsigned char     ( ) const { return std::numeric_limits<unsigned char>::max(); }
};

extern MAYBE_UNUSED PosInfTy inf;
extern MAYBE_UNUSED PosInfTy pos_inf;

struct NaNTy
{
	__forceinline operator double( ) const { return std::numeric_limits<double>::quiet_NaN(); }
	__forceinline operator float ( ) const { return std::numeric_limits<float>::quiet_NaN(); }
};

extern MAYBE_UNUSED NaNTy nan;

struct UlpTy
{
	__forceinline operator double( ) const { return std::numeric_limits<double>::epsilon(); }
	__forceinline operator float ( ) const { return std::numeric_limits<float>::epsilon(); }
};

extern MAYBE_UNUSED UlpTy ulp;

struct PiTy
{
	__forceinline operator double( ) const { return double(M_PI); }
	__forceinline operator float ( ) const { return float(M_PI); }
};

extern MAYBE_UNUSED PiTy pi;

struct OneOverPiTy
{
	__forceinline operator double( ) const { return double(M_1_PI); }
	__forceinline operator float ( ) const { return float(M_1_PI); }
};

extern MAYBE_UNUSED OneOverPiTy one_over_pi;

struct TwoPiTy
{
	__forceinline operator double( ) const { return double(2.0*M_PI); }
	__forceinline operator float ( ) const { return float(2.0*M_PI); }
};

extern MAYBE_UNUSED TwoPiTy two_pi;

struct OneOverTwoPiTy
{
	__forceinline operator double( ) const { return double(0.5*M_1_PI); }
	__forceinline operator float ( ) const { return float(0.5*M_1_PI); }
};

extern MAYBE_UNUSED OneOverTwoPiTy one_over_two_pi;

struct FourPiTy
{
	__forceinline operator double( ) const { return double(4.0*M_PI); } 
	__forceinline operator float ( ) const { return float(4.0*M_PI); }
};

extern MAYBE_UNUSED FourPiTy four_pi;

struct OneOverFourPiTy
{
	__forceinline operator double( ) const { return double(0.25*M_1_PI); }
	__forceinline operator float ( ) const { return float(0.25*M_1_PI); }
};

extern MAYBE_UNUSED OneOverFourPiTy one_over_four_pi;

struct StepTy {
};

extern MAYBE_UNUSED StepTy step;

struct ReverseStepTy {
};

extern MAYBE_UNUSED ReverseStepTy reverse_step;

struct EmptyTy {
};

extern MAYBE_UNUSED EmptyTy empty;

struct FullTy {
};

extern MAYBE_UNUSED FullTy full;

struct UndefinedTy {
};

template <typename T> constexpr auto E               = T(2.71828182845904523536);
template <typename T> constexpr auto LogTwo          = T(0.69314718055994530942);
template <typename T> constexpr auto InvLogTwo       = T(1.44269504088896340736);

template <typename T> constexpr auto Pi              = T(3.14159265358979323846);
template <typename T> constexpr auto InvPi           = T(0.31830988618379067154);
template <typename T> constexpr auto SqrtPi          = T(1.77245385090551602793);
template <typename T> constexpr auto InvSqrtPi       = T(0.56418958354775628695);

template <typename T> constexpr auto TwoPi           = T(6.28318530717958647692);
template <typename T> constexpr auto InvTwoPi        = T(0.15915494309189533577);
template <typename T> constexpr auto SqrtTwoPi       = T(2.50662827463100050242);
template <typename T> constexpr auto InvSqrtTwoPi    = T(0.39894228040143267794);

template <typename T> constexpr auto FourPi          = T(12.5663706143591729539);
template <typename T> constexpr auto InvFourPi       = T(0.07957747154594766788);
template <typename T> constexpr auto SqrtFourPi      = T(3.54490770181103205460);
template <typename T> constexpr auto InvSqrtFourPi   = T(0.28209479177387814347);

template <typename T> constexpr auto SqrtTwo         = T(1.41421356237309504880);
template <typename T> constexpr auto InvSqrtTwo      = T(0.70710678118654752440);

#if defined(__GNUC__)
template <typename T> constexpr auto Infinity        = T(__builtin_inf());
#else
template <typename T> constexpr auto Infinity        = T(__builtin_huge_val());
#endif

template <typename T> constexpr auto NaN             = T(__builtin_nan(""));

template <typename T> constexpr auto Epsilon         = T(sizeof(T) == 8 ? 0x1p-53 : 0x1p-24);

/// 1 - Machine epsilon
template <typename T> constexpr auto OneMinusEpsilon = T(sizeof(T) == 8 ? 0x1.fffffffffffffp-1 : 0x1.fffffep-1);

/// Any numbers below this threshold will overflow to infinity when a reciprocal is evaluated
template <typename T> constexpr auto RecipOverflow   = T(sizeof(T) == 8 ? 0x1p-1024 : 0x1p-128);

/// Smallest normalized floating point value
template <typename T> constexpr auto Smallest        = T(sizeof(T) == 8 ? 0x1p-1022 : 0x1p-126);
template <typename T> constexpr auto Largest         = T(sizeof(T) == 8 ? 0x1.fffffffffffffp+1023 : 0x1.fffffep+127);

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_CONSTANTS_H
