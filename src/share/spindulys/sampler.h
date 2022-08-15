#ifndef SPINDULYS_SAMPLER_H
#define SPINDULYS_SAMPLER_H

#include "../spindulys.h"

#include <algorithm>
#include <random>

#include "./math/vec2.h"
#include "./math/vec3.h"

#include "../spindulys.h"


SPINDULYS_NAMESPACE_OPEN_SCOPE

struct Sampler
{
	std::uniform_real_distribution<float> floatUniformDistribution;
	std::mt19937 prng;

	__forceinline Sampler(): floatUniformDistribution(0.0f, 1.0f), prng(NewSeed()) { }

	__forceinline unsigned NewSeed()
	{
		std::uniform_int_distribution<unsigned> seed;
		std::random_device rng;

		return seed(rng);
	}

	__forceinline float Uniform1D() { return floatUniformDistribution(prng); }
	__forceinline Vec2f Uniform2D() { return Vec2f(Uniform1D(), Uniform1D()); }

	__forceinline std::vector<Vec2f> Stratified2D(int samples)
	{
		std::vector<Vec2f> sample2D;

		sample2D.resize(samples);

		for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx)
			sample2D[sampleIdx][0] = (static_cast<float>(sampleIdx) + Uniform1D()) / samples;

		std::shuffle(sample2D.begin(), sample2D.end(), prng);

		for (int sampleIdx = 0; sampleIdx < samples; ++sampleIdx)
			sample2D[sampleIdx][1] = (static_cast<float>(sampleIdx) + Uniform1D()) / samples;

		return sample2D;
	}

	__forceinline Vec3f HemisphereUniform(float rand0, float rand1)
	{
		const float r = sqrt(1.0f - rand0 * rand0);
		const float theta = M_2_PI * rand1;

		return Vec3f(r * cos(theta), rand0, r * sin(theta));
	}

	__forceinline Vec3f HemisphereCosineWeighted(float rand0, float rand1)
	{
		float r = sqrt(rand0);
		float theta = rand1 * M_2_PI;

		return Vec3f(r * cos(theta), sqrt(std::max(0.0f, 1.0f - rand0)), r * sin(theta));
	}

	__forceinline Vec3f SphereUniform(float rand0, float rand1)
	{
		float theta = rand0 * M_2_PI;
		float phi = std::asin(rand1 * 2.0f - 1.0f);

		return Vec3f(cos(theta) * cos(phi), sin(phi), sin(theta) * cos(phi));
	}

};

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SAMPLER_H
