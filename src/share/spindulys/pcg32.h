/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For additional information about the PCG random number generation scheme,
 * including its license and other licensing options, visit
 *
 *     http://www.pcg-random.org
 */

#ifndef SPINDULYS_PCG32_H
#define SPINDULYS_PCG32_H

#include "../spindulys.h"

#include "math/math.h"
#include "math/vec3.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

static constexpr uint64_t PCG32_DEFAULT_STATE  = 0x853c49e6748fea9bULL;
static constexpr uint64_t PCG32_DEFAULT_STREAM = 0xda3e39cb94b95bdbULL;
static constexpr uint64_t PCG32_MULT           = 0x5851f42d4c957f2dULL;

struct PCG32
{
	PCG32(size_t size = 1,
			const uint64_t& initstate = PCG32_DEFAULT_STATE,
			const uint64_t& initseq   = PCG32_DEFAULT_STREAM)
	{
		Seed(initstate, initseq);
	}

	/**
	 * Seed the pseudorandom number generator
	 *
	 * Specified in two parts: a state initializer and a sequence selection
	 * constant (a.k.a. stream id)
	 */
	void Seed( const uint64_t& initstate = PCG32_DEFAULT_STATE, const uint64_t& initseq = PCG32_DEFAULT_STREAM)
	{
		state = 0;
		inc = (initseq << 1) | 1u;
		NextUInt32();
		state += initstate;
		NextUInt32();
	}

	// Generate a uniformly distributed unsigned 32-bit random number
	uint32_t NextUInt32()
	{
		uint64_t oldstate = state;

		state = madd(oldstate, PCG32_MULT, inc);

		uint32_t xorshifted = static_cast<uint32_t>(((oldstate >> 18) ^ oldstate) >> 27);
		uint32_t rot = static_cast<uint32_t>((oldstate >> 59));

		return (xorshifted >> rot) | (xorshifted << ((-static_cast<uint32_t>(rot)) & 31));

	}

	// Generate a uniformly distributed unsigned 64-bit random number
	uint64_t NextUInt64()
	{
		/* v0, v1 computed as separate statements to ensure a consistent
			 evaluation order across compilers */
		uint32_t v0 = NextUInt32();
		uint32_t v1 = NextUInt32();

		return static_cast<uint64_t>(v0) | (static_cast<uint64_t>(v1) << 32);
	}

	// Forward \ref next_uint call to the correct method based given type size
	template <typename Value, std::enable_if_t<std::is_same_v<Value, uint32_t> || std::is_same_v<Value, uint64_t>, int> = 0>
	Value NextUInt()
	{
		if constexpr (std::is_same_v<Value, uint64_t>)
			return NextUInt64();
		else
			return NextUInt32();
	}

	// Generate a single precision floating point value on the interval [0, 1)
	float NextFloat32()
	{
		return static_cast<float>((NextUInt32() << 9) | 0x3f800000u) - 1.f;
	}

	/**
	 * \brief Generate a double precision floating point value on the interval [0, 1)
	 *
	 * Since the underlying random number generator produces 32 bit output,
	 * only the first 32 mantissa bits will be filled (however, the resolution is still
	 * finer than in NextFloat(), which only uses 23 mantissa bits)
	 */
	double NextFloat64()
	{
		/* Trick from MTGP: generate an uniformly distributed
			 double precision number in [1,2) and subtract 1. */
		return static_cast<double>((static_cast<uint64_t>(NextUInt32()) << 20) |
				0x3ff0000000000000ull) - 1.0;
	}

	/// Forward \ref next_float call to the correct method based given type size
	template <typename Value, std::enable_if_t<std::is_same_v<Value, float> || std::is_same_v<Value, double>> = 0>
	Value NextFloat() {
		 if constexpr (std::is_same_v<Value, double>)
			 return NextFloat64();
		 else
			 return NextFloat32();
	 }

	// Generate a uniformly distributed integer r, where 0 <= r < bound
	uint32_t NextUInt32Bounded(uint32_t bound)
	{
		/* To avoid bias, we need to make the range of the RNG a multiple of
			 bound, which we do by dropping output less than a threshold.
			 A naive scheme to calculate the threshold would be to do

			 uint32_t threshold = 0x1'0000'0000ull % bound;

			 but 64-bit div/mod is slower than 32-bit div/mod (especially on
			 32-bit platforms).  In essence, we do

			 uint32_t threshold = (0x1'0000'0000ull-bound) % bound;

			 because this version will calculate the same modulus, but the LHS
			 value is less than 2^32.
			 */

		uint32_t threshold = (~bound + 1u) % bound;

		/* Uniformity guarantees that this loop will terminate.  In practice, it
			 should usually terminate quickly; on average (assuming all bounds are
			 equally likely), 82.25% of the time, we can expect it to require just
			 one iteration.  In the worst case, someone passes a bound of 2^31 + 1
			 (i.e., 2147483649), which invalidates almost 50% of the range.  In
			 practice, bounds are typically small and only a tiny amount of the range
			 is eliminated.
			 */

		while (true)
			if (const uint32_t result = NextUInt32(); result >= threshold)
				return result % bound;
	}

	// Generate a uniformly distributed integer r, where 0 <= r < bound
	uint64_t NextUInt64Bounded(uint64_t bound)
	{
		uint64_t threshold = (~bound + (uint64_t) 1) % bound;

		while (true)
			if (uint64_t result = NextUInt64(); result >= threshold)
				return result % bound;
	}

	/// Forward \ref next_uint_bounded call to the correct method based given type size
	// Forward \ref next_uint call to the correct method based given type size
	template <typename Value, std::enable_if_t<std::is_same_v<Value, uint32_t> || std::is_same_v<Value, uint64_t>, int> = 0>
	Value NextUIntBounded(Value bound)
	{
		if constexpr (std::is_same_v<Value, uint64_t>)
			return NextUInt64Bounded(bound);
		else
			return NextUInt32Bounded(bound);
	}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4127) /* warning C4127 conditional expression is constant */
#endif

	/**
	 * \brief Multi-step advance function (jump-ahead, jump-back)
	 *
	 * The method used here is based on Brown, "Random Number Generation with
	 * Arbitrary Stride", Transactions of the American Nuclear Society (Nov.
	 * 1994). The algorithm is very similar to fast exponentiation.
	 */
	PCG32 operator+(const int64_t& delta_) const
	{
		uint64_t cur_plus = inc;
		uint64_t acc_mult = 1;
		uint64_t acc_plus = 0;
		uint64_t cur_mult = PCG32_MULT;

		/* Even though delta is an unsigned integer, we can pass a signed
			 integer to go backwards, it just goes "the long way round". */
		uint64_t delta(delta_);

		int it = 0;
		while (delta != 0)
		{
			if ((delta & 1) != 0)
			{
				acc_mult = acc_mult * cur_mult;
				acc_plus = acc_plus * cur_mult + cur_plus;
			}
			cur_plus = (cur_mult + 1) * cur_plus;
			cur_mult *= cur_mult;
			delta = delta >> 1;

			if (++it == 64)
				break;
		}

		return PCG32(initialize_state(), acc_mult * state + acc_plus, inc);
	}

	PCG32 operator-(const int64_t& delta) const
	{
		return operator+(-delta);
	}

	PCG32 &operator+=(const int64_t& delta) { *this = operator+(delta); return *this; }
	PCG32 &operator-=(const int64_t& delta) { *this = operator+(-delta); return *this; }

	/// Compute the distance between two PCG32 pseudorandom number generators
	int64_t operator-(const PCG32 &other) const
	{
		uint64_t cur_plus = inc;
		uint64_t cur_state = other.state;
		uint64_t distance = 0;
		uint64_t bit = 1;
		uint64_t cur_mult = PCG32_MULT;

		int it = 0;
		while (state != cur_state)
		{
			if ((state & bit) != (cur_state & bit))
			{
				cur_state = madd(cur_state, cur_mult, cur_plus);
				distance |= bit;
			}
			cur_plus = (cur_mult + 1) * cur_plus;
			cur_mult *= cur_mult;
			bit = bit << 1;

			if (++it == 64)
				break;
		}

		return int64_t(distance);
	}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

	/**
	 * \brief Draw uniformly distributed permutation and permute the
	 * given container
	 *
	 * From: Knuth, TAoCP Vol. 2 (3rd 3d), Section 3.4.2
	 */
	template <typename Iterator>
		void shuffle(Iterator begin, Iterator end)
		{
			for (Iterator it = end - 1; it > begin; --it)
				std::swap(*it, *(begin + NextUInt32Bounded(static_cast<uint32_t>(it - begin + 1))));
		}

	// Equality operator
	bool operator==(const PCG32 &other) const { return state == other.state && inc == other.inc; }

	// Inequality operator
	bool operator!=(const PCG32 &other) const { return state != other.state || inc != other.inc; }

	uint64_t state;  // RNG state.  All values are possible.
	uint64_t inc;    // Controls which RNG sequence (stream) is selected. Must *always* be odd.

	private:
	struct initialize_state { };
	PCG32(initialize_state, const uint64_t& state, const uint64_t& inc)
		: state(state), inc(inc) { }

};

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_PCG32_H
