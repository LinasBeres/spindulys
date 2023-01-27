#ifndef SPINDULYS_SAMPLER_STRATIFIED_H
#define SPINDULYS_SAMPLER_STRATIFIED_H

#include "sampler.h"

#include "../../spindulys.h"


SPINDULYS_NAMESPACE_OPEN_SCOPE

class StratifiedSampler final : public PCG32Sampler
{
	public:
		StratifiedSampler(bool jitter = true, uint32_t sampleCount = 1, uint32_t baseSeed = 0, uint32_t dimensionIndex = 0, uint32_t sampleIndex = 0)
			: PCG32Sampler(sampleCount, baseSeed, dimensionIndex, sampleIndex), m_jitter(jitter)
		{
			SetSampleCount(sampleCount);
		}

		virtual bool SetSampleCount(uint32_t spp) override
		{
			m_resolution = 1;

			while ((m_resolution * m_resolution) < spp)
				++m_resolution;

			if (spp != (m_resolution * m_resolution))
				spdlog::warn("Sample count should be square and power of two, rounding to {}", m_resolution);

			m_sampleCount = m_resolution * m_resolution;
			m_invSampleCount = rcp(static_cast<float>(m_sampleCount));
			m_invResolution = rcp(static_cast<float>(m_resolution));

			return true;
		}

		virtual Sampler* Fork() override
		{
			StratifiedSampler *sampler = new StratifiedSampler();
			sampler->m_jitter         = m_jitter;
			sampler->m_sampleCount    = m_sampleCount;
			sampler->m_invSampleCount = m_invSampleCount;
			sampler->m_resolution     = m_resolution;
			sampler->m_invResolution  = m_invResolution;
			sampler->m_baseSeed       = m_baseSeed;
			return dynamic_cast<Sampler*>(sampler);
		}

		virtual Sampler* Clone() override
		{
			return new StratifiedSampler(*this);
		}

		virtual void Seed(uint32_t seed) override
		{
			PCG32Sampler::Seed(seed);
			m_permutationSeed = ComputePerSequenceSeed(seed);
		}

		virtual float Next1d(bool active = true) override
		{
			assert(Seeded());

			uint32_t sampleIndices = GetCurrentSampleIndex();
			uint32_t permSeed = m_permutationSeed + m_dimensionIndex++;

			// Shuffle the samples order
			uint32_t p = PermuteKensler(sampleIndices, m_sampleCount, permSeed, active);

			// Add a random perturbation
			float j = m_jitter ? m_rng.NextFloat32() : .5f;

			return (p + j) * m_invSampleCount;
		}

		virtual Vec2f Next2d(bool active = true) override
		{
			assert(Seeded());

			uint32_t sampleIndices = GetCurrentSampleIndex();
			uint32_t permSeed = m_permutationSeed + m_dimensionIndex++;

			// Shuffle the samples order
			uint32_t p = PermuteKensler(sampleIndices, m_sampleCount, permSeed, active);

			// Map the index to its 2D cell
			uint32_t y = p * m_invResolution;  // p / m_resolution
			uint32_t x = p - y * m_resolution; // p % m_resolution

			// Add a random perturbation
			float jx = .5f;
			float jy = .5f;
			if (m_jitter)
			{
				jx = m_rng.NextFloat32();
				jy = m_rng.NextFloat32();
			}

			// Construct the final 2D point
			return Vec2f(x + jx, y + jy) * m_invResolution;
		}

	private:
		StratifiedSampler(const StratifiedSampler& sampler)
			: PCG32Sampler(sampler)
		{
			m_jitter          = sampler.m_jitter;
			m_resolution      = sampler.m_resolution;
			m_invResolution   = sampler.m_invResolution;
			m_invSampleCount  = sampler.m_invSampleCount;
			m_permutationSeed = sampler.m_permutationSeed;
		}

	private:
		bool m_jitter = true;

		uint32_t m_resolution = 1;
		float m_invResolution = 1.f;
		float m_invSampleCount = 1.f;

		/// Per-sequence permutation seed
		uint32_t m_permutationSeed = 0;
};

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_SAMPLER_STRATIFIED_H
