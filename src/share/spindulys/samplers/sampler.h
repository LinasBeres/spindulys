#ifndef SPINDULYS_SAMPLER_H
#define SPINDULYS_SAMPLER_H

#include "../math/vec2.h"

#include "../random.h"
#include "../pcg32.h"

#include "../../spindulys.h"


SPINDULYS_NAMESPACE_OPEN_SCOPE

class Sampler
{
	public:
		/**
		 * \brief Create a fork of this sampler.
		 *
		 * A subsequent call to \c seed is necessary to properly initialize
		 * the internal state of the sampler.
		 *
		 * May throw an exception if not supported.
		 */
		virtual Sampler* Fork() = 0;

		/**
		 * \brief Create a clone of this sampler.
		 *
		 * Subsequent calls to the cloned sampler will produce the same
		 * random numbers as the original sampler.
		 *
		 * \remark This method relies on the overload of the copy constructor.
		 *
		 * May throw an exception if not supported.
		 */
		virtual Sampler* Clone() = 0;

		/**
		 * Deterministically seed the underlying RNG, if applicable.
		 *
		 */
		virtual void Seed(uint32_t /* seed */)
		{
			m_dimensionIndex = 0;
			m_sampleIndex = 0;
			m_seeded = true;
		}

		/**
		 * \brief Advance to the next sample.
		 *
		 * A subsequent call to \c next_1d or \c next_2d will access the first
		 * 1D or 2D components of this sample.
		 */
		virtual void Advance()
		{
			m_dimensionIndex = 0;
			++m_sampleIndex;
		}

		// Retrieve the next component value from the current sample
		virtual float Next1d(bool active = true) = 0;

		// Retrieve the next two component values from the current sample
		virtual Vec2f Next2d(bool active = true) = 0;

		// Get Methods
		uint32_t GetSampleCount() const { return m_sampleCount; }

		// Set the number of samples per pixel
		virtual bool SetSampleCount(uint32_t spp) { return spp != std::exchange(m_sampleCount, spp); }

		// Return whether the sampler was seeded
		bool Seeded() const { return m_seeded; }

	protected:
		Sampler(uint32_t sampleCount = 4, uint32_t baseSeed = 0, uint32_t dimensionIndex = 0, uint32_t sampleIndex = 0)
		{
			m_sampleCount    = sampleCount;
			m_baseSeed       = baseSeed;
			m_dimensionIndex = dimensionIndex;
			m_sampleIndex    = sampleIndex;
		}
		Sampler(const Sampler& sampler)
		{
			m_sampleCount          = sampler.m_sampleCount;
			m_baseSeed             = sampler.m_baseSeed;
			m_dimensionIndex       = sampler.m_dimensionIndex;
			m_sampleIndex          = sampler.m_sampleIndex;
		}

		virtual ~Sampler() {}

		// Generates a array of seeds where the seed values are unique per sequence
		uint32_t ComputePerSequenceSeed(uint32_t seed) const { return SampleTea32(m_baseSeed, seed).first; }

		// Return the index of the current sample
		uint32_t GetCurrentSampleIndex() const { return m_sampleIndex; }

	protected:
		// Base seed value
		uint32_t m_baseSeed = 0;

		// Number of samples per pixel
		uint32_t m_sampleCount = 4;

		// Index of the current dimension in the sample
		uint32_t m_dimensionIndex = 0;

		// Index of the current sample in the sequence
		uint32_t m_sampleIndex = 0;

		// Seeded check
		bool m_seeded = false;
};

class PCG32Sampler : public Sampler
{
	public:
		virtual void Seed(uint32_t seed) override
		{
			uint32_t seed_value = m_baseSeed + seed;
			m_rng.Seed(seed_value, PCG32_DEFAULT_STREAM);
		}

	protected:
		PCG32Sampler(uint32_t sampleCount = 4, uint32_t baseSeed = 0, uint32_t dimensionIndex = 0, uint32_t sampleIndex = 0)
			: Sampler(sampleCount, baseSeed, dimensionIndex, sampleIndex)
		{ }

		PCG32Sampler(const PCG32Sampler& sampler)
			: Sampler(sampler)
		{
			m_rng = sampler.m_rng;
		}

	protected:
		PCG32 m_rng;
};

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SAMPLER_H
