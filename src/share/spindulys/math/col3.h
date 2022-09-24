#ifndef SPINDULYS_COL3_H
#define SPINDULYS_COL3_H

#include <cstddef>
#include <assert.h>
#include <iostream>

#include "../../spindulys.h"
#include "../platform.h"

#include "constants.h"
#include "math.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

////////////////////////////////////////////////////////////////////////////////
/// RGB Color Class
////////////////////////////////////////////////////////////////////////////////

template<typename T> struct Col3
{
	T r, g, b;

	////////////////////////////////////////////////////////////////////////////////
	/// Construction
	////////////////////////////////////////////////////////////////////////////////

	__forceinline Col3           ( )                   { }
	__forceinline Col3           ( const Col3& other ) { r = other.r; g = other.g; b = other.b; }
	__forceinline Col3& operator=( const Col3& other ) { r = other.r; g = other.g; b = other.b; return *this; }

	__forceinline explicit Col3 (const T& v)                         : r(v), g(v), b(v) {}
	__forceinline          Col3 (const T& r, const T& g, const T& b) : r(r), g(g), b(b) {}

	////////////////////////////////////////////////////////////////////////////////
	/// Constants
	////////////////////////////////////////////////////////////////////////////////

	__forceinline Col3 (ZeroTy)   : r(zero)   , g(zero)   , b(zero)    {}
	__forceinline Col3 (OneTy)    : r(one)    , g(one)    , b(one)     {}
	__forceinline Col3 (PosInfTy) : r(pos_inf), g(pos_inf), b(pos_inf) {}
	__forceinline Col3 (NegInfTy) : r(neg_inf), g(neg_inf), b(neg_inf) {}
};

////////////////////////////////////////////////////////////////////////////////
/// Unary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline Col3<T> operator +( const Col3<T>& a ) { return Col3<T>(+a.r, +a.g, +a.b); }
template<typename T> __forceinline Col3<T> operator -( const Col3<T>& a ) { return Col3<T>(-a.r, -a.g, -a.b); }

template<typename T> __forceinline Col3<T> toRGB     ( const Col3<T>& a ) { return clamp(a) * 255.f; }
template<typename T> __forceinline Col3<T> toSRGB    ( const Col3<T>& a ) { return pow(a, 1.f / 2.2f); }
template<typename T> __forceinline Col3<T> toLinear  ( const Col3<T>& a ) { return pow(a, 2.2f); }

////////////////////////////////////////////////////////////////////////////////
/// Binary Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline Col3<T> operator +( const Col3<T>& a, const Col3<T>& b ) { return Col3<T>(a.r + b.r, a.g + b.g, a.b + b.b); }
template<typename T> __forceinline Col3<T> operator -( const Col3<T>& a, const Col3<T>& b ) { return Col3<T>(a.r - b.r, a.g - b.g, a.b - b.b); }
template<typename T> __forceinline Col3<T> operator *( const Col3<T>& a, const Col3<T>& b ) { return Col3<T>(a.r * b.r, a.g * b.g, a.b * b.b); }
template<typename T> __forceinline Col3<T> operator *( const       T& a, const Col3<T>& b ) { return Col3<T>(a   * b.r, a   * b.g, a   * b.b); }
template<typename T> __forceinline Col3<T> operator *( const Col3<T>& a, const       T& b ) { return Col3<T>(a.r * b  , a.g * b  , a.b * b  ); }
template<typename T> __forceinline Col3<T> operator /( const Col3<T>& a, const       T& b ) { return Col3<T>(a.r / b  , a.g / b  , a.b / b  ); }
template<typename T> __forceinline Col3<T> operator /( const       T& a, const Col3<T>& b ) { return Col3<T>(a   / b.r, a   / b.g, a   / b.b); }
template<typename T> __forceinline Col3<T> operator /( const Col3<T>& a, const Col3<T>& b ) { return Col3<T>(a.r / b.r, a.g / b.g, a.b / b.b); }

template<typename T> __forceinline Col3<T> min(const Col3<T>& a, const Col3<T>& b) { return Col3<T>(min(a.r, b.r), min(a.g, b.g), min(a.b, b.b)); }
template<typename T> __forceinline Col3<T> max(const Col3<T>& a, const Col3<T>& b) { return Col3<T>(max(a.r, b.r), max(a.g, b.g), max(a.b, b.b)); }

template<typename T> __forceinline Col3<T> operator >>( const Col3<T>& a, const int b ) { return Col3<T>(a.r >> b, a.g >> b, a.b >> b); }
template<typename T> __forceinline Col3<T> operator <<( const Col3<T>& a, const int b ) { return Col3<T>(a.r << b, a.g << b, a.b << b); }

template<typename T> __forceinline Col3<T> pow(const Col3<T>& a, const T& b) { return Col3<T>(pow(a.r, b), pow(a.g, b), pow(a.b, b)); }

////////////////////////////////////////////////////////////////////////////////
/// Assignment Operators
////////////////////////////////////////////////////////////////////////////////

template<typename T> __forceinline Col3<T>& operator +=( Col3<T>& a, const T        b ) { a.r += b;   a.g += b;   a.b += b;   return a; }
template<typename T> __forceinline Col3<T>& operator +=( Col3<T>& a, const Col3<T>& b ) { a.r += b.r; a.g += b.g; a.b += b.b; return a; }
template<typename T> __forceinline Col3<T>& operator -=( Col3<T>& a, const Col3<T>& b ) { a.r -= b.r; a.g -= b.g; a.b -= b.b; return a; }
template<typename T> __forceinline Col3<T>& operator *=( Col3<T>& a, const       T& b ) { a.r *= b  ; a.g *= b  ; a.b *= b  ; return a; }
template<typename T> __forceinline Col3<T>& operator /=( Col3<T>& a, const       T& b ) { a.r /= b  ; a.g /= b  ; a.b /= b  ; return a; }

////////////////////////////////////////////////////////////////////////////////
/// Comparison Operators
////////////////////////////////////////////////////////////////////////////////

template<typename L> __forceinline bool operator ==( const Col3<L>& a, const Col3<L>& b ) { return a.r == b.r && a.g == b.g && a.b == b.b; }
template<typename L> __forceinline bool operator !=( const Col3<L>& a, const Col3<L>& b ) { return a.r != b.r || a.g != b.g || a.b != b.b; }

/*! output operator */
template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Col3<T>& a) {
	return cout << "(" << a.r << ", " << a.g << ", " << a.b << ")";
}

/*! default template instantiations */
typedef Col3<unsigned char> Col3uc;
typedef Col3<float        > Col3f;

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif //SPINDULYS_COL3_H
