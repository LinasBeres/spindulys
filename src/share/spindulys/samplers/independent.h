#ifndef SPINDULYS_SAMPLER_INDEPENDENT_H
#define SPINDULYS_SAMPLER_INDEPENDENT_H

#include "sampler.h"

#include "../math/vec3.h"

#include "../../spindulys.h"


SPINDULYS_NAMESPACE_OPEN_SCOPE

class IndependentSampler final : public PCG32Sampler
{
	public:
		IndependentSampler(uint32_t sampleCount = 1, uint32_t baseSeed = 0, uint32_t dimensionIndex = 0, uint32_t sampleIndex = 0)
			: PCG32Sampler(sampleCount, baseSeed, dimensionIndex, sampleIndex)
		{
		}

		virtual Sampler* Fork() override
		{
			IndependentSampler *sampler = new IndependentSampler();
			sampler->m_sampleCount    = m_sampleCount;
			sampler->m_baseSeed       = m_baseSeed;
			return dynamic_cast<Sampler*>(sampler);
		}

		virtual Sampler* Clone() override
		{
			return new IndependentSampler(*this);
		}

		virtual float Next1d(bool active = true) override
		{
			assert(Seeded());

			return m_rng.NextFloat32();
		}

		virtual Vec2f Next2d(bool active = true) override
		{
			assert(Seeded());

			return Vec2f(Next1d(), Next1d());
		}

	private:
		IndependentSampler(const IndependentSampler& sampler)
			: PCG32Sampler(sampler)
		{ }

	private:
};


SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_SAMPLER_INDEPENDENT_H
