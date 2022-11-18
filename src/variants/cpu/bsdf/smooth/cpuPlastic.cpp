#include "cpuPlastic.h"

#include <spindulys/fresnel.h>

#include "../../utils/interaction.h"

CPU_NAMESPACE_OPEN_SCOPE

CPUSmoothPlastic::CPUSmoothPlastic(float interiorIOR, float exteriorIOR,
				const Col3f& diffuseReflection, const Col3f& specularReflectance,
				bool nonLinear, const std::string& id)
	: SmoothPlastic(interiorIOR, exteriorIOR, diffuseReflection, specularReflectance, nonLinear, id)
{
}

std::pair<BSDFSample, Col3f>
CPUSmoothPlastic::Sample(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		float sample1,
		const Vec2f& sample2,
		uint32_t active) const
{
	BSDFSample bs;

	return { bs, zero };
}

Col3f CPUSmoothPlastic::Eval(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return zero;
}

float CPUSmoothPlastic::Pdf(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return zero;
}

CPU_NAMESPACE_CLOSE_SCOPE
