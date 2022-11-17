#ifndef CPU_DIELECTRIC_H
#define CPU_DIELECTRIC_H

#include <bsdf/smooth/dielectric.h>

#include "../cpuBSDF.h"

#include "../../spindulysCPU.h"

CPU_NAMESPACE_OPEN_SCOPE

class CPUSmoothDielectric final : public CPUBSDF, public SmoothDielectric
{
	public:
		CPUSmoothDielectric(float interiorIOR, float exteriorIOR,
				const Col3f& specularReflectance, const Col3f& specularTransmittance,
				const std::string& id = "defaultSmoothDielectric");
		~CPUSmoothDielectric() = default;

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
