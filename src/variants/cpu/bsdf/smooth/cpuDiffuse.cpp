#include "cpuDiffuse.h"

#include <spindulys/math/warp.h>

#include "../../utils/interaction.h"

CPU_NAMESPACE_OPEN_SCOPE

CPUSmoothDiffuse::CPUSmoothDiffuse(const Col3f& reflectance, const std::string& id)
	: SmoothDiffuse(reflectance, id)
{
}

std::pair<BSDFSample, Col3f>
CPUSmoothDiffuse::Sample(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		float sample1,
		const Vec2f& sample2,
		uint32_t active) const
{
	float cos_theta_i = si.wi.z;
	BSDFSample bs;

	active &= cos_theta_i > 0.f;
	// if (!active || !ctx.IsEnabled(BSDFFlags::DiffuseReflection))
		// return { bs, zero };

	bs.wo = square_to_cosine_hemisphere(sample2);
	bs.pdf = square_to_cosine_hemisphere_pdf(bs.wo);
	bs.eta = 1.f;
	bs.sampledType = +BSDFFlags::DiffuseReflection;
	bs.sampledComponent = 0;

	return { bs, (active && bs.pdf > 0.f) ? m_reflectance : zero };

}

Col3f CPUSmoothDiffuse::Eval(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	if (!ctx.IsEnabled(BSDFFlags::DiffuseReflection))
		return zero;

	const float cos_theta_i = si.wi.z;
	const float cos_theta_o = wo.z;

	active &= cos_theta_i > 0.f && cos_theta_o > 0.f;

	Col3f value = active ? m_reflectance * InvPi<float> * cos_theta_o : zero;

	return value;
}

float CPUSmoothDiffuse::Pdf(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	if (!ctx.IsEnabled(BSDFFlags::DiffuseReflection))
		return 0.f;

	const float cos_theta_i = si.wi.z;
	const float cos_theta_o = wo.z;

	const float pdf = square_to_cosine_hemisphere_pdf(wo);

	return select(cos_theta_i > 0.f && cos_theta_o > 0.f, pdf, 0.f);
}

CPU_NAMESPACE_CLOSE_SCOPE
