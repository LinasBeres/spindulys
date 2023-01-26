#ifndef SPINDULYS_RANDOM_H
#define SPINDULYS_RANDOM_H

#include "../spindulys.h"

#include "math/math.h"
#include "math/vec3.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

/**
 * Generate fast and reasonably good pseudorandom numbers using the
 * Tiny Encryption Algorithm (TEA) by David Wheeler and Roger Needham.
 *
 * For details, refer to "GPU Random Numbers via the Tiny Encryption Algorithm"
 * by Fahad Zafar, Marc Olano, and Aaron Curtis.
 *
 * \param v0
 *     First input value to be encrypted (could be the sample index)
 *
 * \param v1
 *     Second input value to be encrypted (e.g. the requested random number dimension)
 *
 * \param rounds
 *     How many rounds should be executed? The default for random number
 *     generation is 4.
 *
 * \return
 *     Two uniformly distributed 32-bit integers
 */

	__forceinline
std::pair<uint32_t, uint32_t> SampleTea32(uint32_t v0, uint32_t v1, int rounds = 4)
{
	uint32_t sum = 0;
	for (int i = 0; i < rounds; ++i)
	{
		sum += 0x9e3779b9;
		v0 += ((v1 << 4) + 0xa341316c) ^ (v1 + sum) ^ ((v1 >> 5) + 0xc8013ea4);
		v1 += ((v0 << 4) + 0xad90777d) ^ (v0 + sum) ^ ((v0 >> 5) + 0x7e95761e);
	}

	return { v0, v1 };
}

	__forceinline
uint64_t SampleTea64(uint32_t v0, uint32_t v1, int rounds = 4)
{
	std::tie(v0, v1) = SampleTea32(v0, v1, rounds);

	return v0 + (static_cast<uint64_t>(v1) << 32);
}

/**
 * Generate pseudorandom permutation vector using the algorithm described in Pixar's
 * technical memo "Correlated Multi-Jittered Sampling":
 *
 *     https://graphics.pixar.com/library/MultiJitteredSampling/
 *
 *  Unlike permute, this function supports permutation vectors of any length.
 *
 * \param index
 *     Input index to be mapped
 * \param sample_count
 *     Length of the permutation vector
 * \param seed
 *     Seed value used as second input to the Tiny Encryption Algorithm. Can be used to
 *     generate different permutation vectors.
 * \return
 *     The index corresponding to the input index in the pseudorandom permutation vector.
 */

__forceinline
uint32_t PermuteKensler(uint32_t index, uint32_t sampleCount, uint32_t seed, bool active = true)
{
	if (sampleCount == 1)
		return 0;

	uint32_t w = sampleCount - 1;
	w |= w >> 1;
	w |= w >> 2;
	w |= w >> 4;
	w |= w >> 8;
	w |= w >> 16;

	auto body = [&seed, &w](uint32_t index) {
		uint32_t tmp = index;
		tmp ^= seed;
		tmp *= 0xe170893d;
		tmp ^= seed >> 16;
		tmp ^= (tmp & w) >> 4;
		tmp ^= seed >> 8;
		tmp *= 0x0929eb3f;
		tmp ^= seed >> 23;
		tmp ^= (tmp & w) >> 1;
		tmp *= 1 | seed >> 27;
		tmp *= 0x6935fa69;
		tmp ^= (tmp & w) >> 11;
		tmp *= 0x74dcb303;
		tmp ^= (tmp & w) >> 2;
		tmp *= 0x9e501cc3;
		tmp ^= (tmp & w) >> 2;
		tmp *= 0xc860a3df;
		tmp &= w;
		tmp ^= tmp >> 5;
		return tmp;
	};

	uint32_t iter = 0, max_iter = 0;
	do {
		if (active)
			index = body(index);
		active &= (index >= sampleCount);
	} while (active || (max_iter > ++iter));

	return (index + seed) % sampleCount;
}

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_RANDOM_H
