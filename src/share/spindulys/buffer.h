#ifndef SPINDULYS_BUFFER_H
#define SPINDULYS_BUFFER_H

#include <vector>

#include "../spindulys.h"

#include "./math/col3.h"
#include "./math/col4.h"


SPINDULYS_NAMESPACE_OPEN_SCOPE

template<typename T> struct Buffer
{
	std::vector<T> _pixelData;

	__forceinline Buffer           (size_t width, size_t height) { _pixelData.resize(width * height); }
	__forceinline Buffer           (const Buffer& other)         { _pixelData = other._pixelData; }
	__forceinline Buffer& operator=(const Buffer& other)         { _pixelData = other._pixelData; return *this; }

	__forceinline void Resize(size_t width, size_t height) { _pixelData.resize(width * height); }

	__forceinline void Clean(int width, int height)
	{
		_pixelData.clear();
		_pixelData.shrink_to_fit();
		_pixelData.resize(width * height);
	}

	__forceinline void Swap(Buffer& buffer) { _pixelData.swap(buffer._pixelData); }

	__forceinline const std::vector<T>& GetPixelData() const { return _pixelData; }

	__forceinline void SetPixel(size_t pixelIndex, const T& pixel) { _pixelData[pixelIndex] = pixel; }
	__forceinline void AddPixel(size_t pixelIndex, const T& a) { _pixelData[pixelIndex] += a; }
	__forceinline void MultiplyPixel(size_t pixelIndex, const float& a) { _pixelData[pixelIndex] *= a; }

	__forceinline const T& GetPixel(size_t pixelIndex) const { return _pixelData[pixelIndex]; }
};

/*! output operator */
template<typename T> inline std::ostream& operator<<(std::ostream& cout, const Buffer<T>& a)
{
	std::string out;
	for (const auto& col : a.GetPixelData())
		out += std::to_string(col.r) + std::to_string(col.g) + std::to_string(col.b) + " ";
	return cout << a;
}

/*! default template instantiations */
typedef Buffer<Col3f> Buffer3f;
typedef Buffer<Col4f> Buffer4f;

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_BUFFER_H
