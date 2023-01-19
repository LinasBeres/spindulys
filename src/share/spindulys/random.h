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

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_RANDOM_H
