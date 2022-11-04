#include "diffuse.h"

#include <spindulys/math/warp.h>

#include "../utils/interaction.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

SmoothDiffuse::SmoothDiffuse(const Col3f& reflectance, const std::string& id)
	: BSDF(id)
	, m_reflectance(reflectance)
{
	m_flags = (uint32_t) BSDFFlags::DiffuseReflection | (uint32_t) BSDFFlags::FrontSide;
	m_components.push_back(m_flags);
}

std::pair<BSDFSample3, Col3f>
SmoothDiffuse::Sample(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		float sample1,
		const Vec2f& sample2,
		uint32_t active) const
{
	// float cos_theta_i = si.wi.z;
	BSDFSample3 bs;

	// active &= cos_theta_i > 0.f;
	// if (!active || !ctx.IsEnabled(BSDFFlags::DiffuseReflection))
		// return { bs, zero };

	bs.wo = square_to_cosine_hemisphere(sample2);
	bs.pdf = square_to_cosine_hemisphere_pdf(bs.wo);
	bs.eta = 1.f;
	bs.sampled_type = (uint32_t) BSDFFlags::DiffuseReflection;
	bs.sampled_component = 0;

	return { bs, (active && bs.pdf > 0.f) ? m_reflectance : zero };

}

Col3f SmoothDiffuse::Eval(const BSDFContext& ctx,
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

float SmoothDiffuse::Pdf(const BSDFContext& ctx,
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

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
