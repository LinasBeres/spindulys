#ifndef CPU_CONDUCTOR_H
#define CPU_CONDUCTOR_H

#include <bsdf/smooth/conductor.h>

#include "../cpuBSDF.h"

#include "../../spindulysCPU.h"

CPU_NAMESPACE_OPEN_SCOPE

class CPUSmoothConductor final : public CPUBSDF, public SmoothConductor
{
	public:
		CPUSmoothConductor(float eta, float k, const Col3f& specularReflectance,
				const std::string& id = "defaultSmoothConductor");
		~CPUSmoothConductor() = default;

		virtual std::pair<BSDFSample3, Col3f>
		Sample(const BSDFContext& ctx,
					const SurfaceInteraction& si,
					float sample1,
					const Vec2f& sample2,
					uint32_t active = true) const override;

		virtual Col3f Eval(const BSDFContext& ctx,
				const SurfaceInteraction& si,
				const Vec3f& wo,
				uint32_t active = true) const override;

		virtual float Pdf(const BSDFContext& ctx,
				const SurfaceInteraction& si,
				const Vec3f& wo,
				uint32_t active = true) const override;

	private:
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_CONDUCTOR_H
