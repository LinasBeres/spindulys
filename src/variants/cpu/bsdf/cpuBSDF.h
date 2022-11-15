#ifndef CPU_BSDF_H
#define CPU_BSDF_H

#include <string>

#include <spindulys/math/vec3.h>
#include <spindulys/sampler.h>

#include <utils/helperStructs.h>

#include <bsdf/bsdf.h>

#include "../spindulysCPU.h"

CPU_NAMESPACE_OPEN_SCOPE

struct SurfaceInteraction;

class CPUBSDF : virtual public BSDF
{
	public:
		virtual ~CPUBSDF() = default;

		virtual std::pair<BSDFSample3, Col3f>
		Sample(const BSDFContext& ctx,
					const SurfaceInteraction& si,
					float sample1,
					const Vec2f& sample2,
					uint32_t active = true) const = 0;

		virtual Col3f Eval(const BSDFContext& ctx,
				const SurfaceInteraction& si,
				const Vec3f& wo,
				uint32_t active = true) const = 0;

		virtual float Pdf(const BSDFContext& ctx,
				const SurfaceInteraction& si,
				const Vec3f& wo,
				uint32_t active = true) const = 0;

		virtual std::pair<Col3f, float>
		EvalPdf(const BSDFContext& ctx,
					const SurfaceInteraction& si,
					const Vec3f& wo,
					uint32_t active = true) const;

		virtual float EvalNullTransmission(const SurfaceInteraction& si,
				uint32_t active = true) const;

	protected:
		CPUBSDF(const std::string& id);
		CPUBSDF() = default;

	protected:

	private:
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_BSDF_H
