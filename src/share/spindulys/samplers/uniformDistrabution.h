#ifndef SPINDULYS_SAMPLER_UNIFORM_H
#define SPINDULYS_SAMPLER_UNIFORM_H

#include <algorithm>
#include <random>

#include "sampler.h"

#include "../math/vec3.h"

#include "../../spindulys.h"


SPINDULYS_NAMESPACE_OPEN_SCOPE

class UniformSampler final : public Sampler
{
	public:
		UniformSampler(uint32_t sampleCount = 1, uint32_t baseSeed = 0, uint32_t dimensionIndex = 0, uint32_t sampleIndex = 0)
			: Sampler(sampleCount, baseSeed, dimensionIndex, sampleIndex)
		{
		}

		virtual Sampler* Fork() override
		{
			UniformSampler *sampler = new UniformSampler();
			sampler->m_sampleCount    = m_sampleCount;
			sampler->m_baseSeed       = m_baseSeed;
			return dynamic_cast<Sampler*>(sampler);
		}

		virtual Sampler* Clone() override
		{
			return new UniformSampler(*this);
		}

		virtual void Seed(uint32_t) override
		{
			std::uniform_int_distribution<unsigned> seed;
			std::random_device rng;

			prng = std::mt19937(seed(rng));
			m_seeded = true;
		}

		virtual float Next1d(bool active = true) override
		{
			assert(Seeded());

			return floatUniformDistribution(prng);
		}

		virtual Vec2f Next2d(bool active = true) override
		{
			assert(Seeded());

			return {Next1d(), Next1d()};
		}

	private:
		UniformSampler(const UniformSampler& sampler)
			: Sampler(sampler)
		{ }

	private:
		std::uniform_real_distribution<float> floatUniformDistribution;
		std::mt19937 prng;
};


SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_SAMPLER_INDEPENDENT_H
