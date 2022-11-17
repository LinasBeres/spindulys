#ifndef CPU_PLASTIC_H
#define CPU_PLASTIC_H

#include <bsdf/smooth/plastic.h>

#include "../cpuBSDF.h"

#include "../../spindulysCPU.h"

CPU_NAMESPACE_OPEN_SCOPE

class CPUSmoothPlastic final : public CPUBSDF, public SmoothPlastic
{
	public:
		CPUSmoothPlastic(float interiorIOR, float exteriorIOR,
				const Col3f& diffuseReflection, const Col3f& specularReflectance,
				bool nonLinear, const std::string& id = "defaultSmoothDielectric");
		~CPUSmoothPlastic() = default;

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

#endif // CPU_DIELECTRIC_H
