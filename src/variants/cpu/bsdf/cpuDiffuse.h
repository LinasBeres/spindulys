#ifndef CPU_DIFFUSE_H
#define CPU_DIFFUSE_H

#include <bsdf/diffuse.h>

#include "cpuBSDF.h"

#include "../spindulysCPU.h"

CPU_NAMESPACE_OPEN_SCOPE

class CPUSmoothDiffuse final : public CPUBSDF, public SmoothDiffuse
{
	public:
		CPUSmoothDiffuse(const Col3f& reflectance, const std::string& id = "defaultSmoothDiffuse");
		~CPUSmoothDiffuse() = default;

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

#endif // CPU_DIFFUSE_H
