#ifndef HELPER_STRUCTS_H
#define HELPER_STRUCTS_H

#include <spindulys/math/vec2.h>
#include <spindulys/math/vec3.h>
#include <spindulys/math/linearspace3.h>
#include <spindulys/sampler.h>

#include "../geometry/geometry.h"

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

static constexpr float errorBias = 32.0f * 1.19209e-07f;

struct PixelSample
{
	SSampler& sampler;   // Sampler to use to generate any random numbers in this sample.
	int pixelX = 0;     // Position of the pixel being sampled on the X-axis.
	int pixelY = 0;     // Position of the pixel being sampled on the Y-axis.
	int pixelIdx = 0;   // Index of the pixel being sampled, from 0 to (width * height).
	int samples = 0;    // Total number of samples per pixel to compute.
	int sampleIdx = 0;  // Keep track of how many samples have been computed.

	__forceinline PixelSample(SSampler& sampler, int pixelX, int pixelY, int pixelIdx, int samples, int sampleIdx)
		: sampler(sampler), pixelX(pixelX), pixelY(pixelY), pixelIdx(pixelIdx), samples(samples), sampleIdx(sampleIdx)
	{ }
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // CPU_HELPER_STRUCTS_H
