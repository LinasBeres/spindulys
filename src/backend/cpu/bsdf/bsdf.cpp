#include "bsdf.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

BSDF::BSDF(const std::string& id)
	: m_flags((uint32_t)BSDFFlags::Empty)
	, m_id(id)
{ }

std::pair<Col3f, float>
BSDF::EvalPdf(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return { Eval(ctx, si, wo, active), Pdf(ctx, si, wo, active) };
}

float BSDF::EvalNullTransmission(const SurfaceInteraction&, uint32_t) const
{
	return 0.f;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
