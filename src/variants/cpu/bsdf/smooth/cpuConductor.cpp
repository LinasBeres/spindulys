#include "cpuConductor.h"

#include <spindulys/fresnel.h>

#include "../../utils/interaction.h"

CPU_NAMESPACE_OPEN_SCOPE

CPUSmoothConductor::CPUSmoothConductor(float eta, float k, const Col3f& specularReflectance, const std::string& id)
	: SmoothConductor(eta, k, specularReflectance, id)
{
}

std::pair<BSDFSample, Col3f>
CPUSmoothConductor::Sample(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		float sample1,
		const Vec2f& sample2,
		uint32_t active) const
{
	float cosThetaI = si.wi.z;
	active &= cosThetaI > 0.f;

	BSDFSample bs;
	Col3f value(zero);

	if (!active || !ctx.IsEnabled(BSDFFlags::DeltaReflection))
		return { bs, value };

	bs.sampledComponent = 0;
	bs.sampledType = +BSDFFlags::DeltaReflection;
	bs.wo  = Reflect(si.wi);
	bs.eta = 1.f;
	bs.pdf = 1.f;

	value = GetSpecularReflectance() * FresnelConductor(cosThetaI, GetETA(), GetK());

	return { bs, value };
}

Col3f CPUSmoothConductor::Eval(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return zero;
}

float CPUSmoothConductor::Pdf(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return zero;
}

CPU_NAMESPACE_CLOSE_SCOPE
