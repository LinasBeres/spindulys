#ifndef SPINDULYS_SAMPLER_H
#define SPINDULYS_SAMPLER_H

#include "../math/vec3.h"

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
		virtual Sampler* fork() = 0;

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
		virtual Sampler* clone() = 0;

		/**
		 * \brief Deterministically seed the underlying RNG, if applicable.
		 *
		 * In the context of wavefront ray tracing & dynamic arrays, this function
		 * must be called with \c wavefront_size matching the size of the wavefront.
		 */
		virtual void seed(uint32_t seed,
				uint32_t wavefront_size = (uint32_t) -1);

	protected:
		Sampler(const Sampler&);
		virtual ~Sampler();

		// Generates a array of seeds where the seed values are unique per sequence
		uint32_t ComputePerSequenceSeed(uint32_t seed) const;
		// Return the index of the current sample
		uint32_t CurrentSampleIndex() const;

	protected:
		// Base seed value
		uint32_t m_baseSeed = 0;

		// Number of samples per pixel
		uint32_t m_sampleCount = 4;

		// Number of samples per pass in wavefront modes (default is 1)
		uint32_t m_samplesPerWavefront = 1;

		// Size of the wavefront (or 0, if not seeded)
		uint32_t m_wavefrontSize = 0;

		// Index of the current dimension in the sample
		uint32_t m_dimensionIndex = 0;

		// Index of the current sample in the sequence
		uint32_t m_sampleIndex = 0;
};

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SAMPLER_H
