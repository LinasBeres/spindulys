#include "cpuBSDF.h"

CPU_NAMESPACE_OPEN_SCOPE

CPUBSDF::CPUBSDF(const std::string& id)
	: BSDF(id)
{ }

std::pair<Col3f, float>
CPUBSDF::EvalPdf(const BSDFContext& ctx,
		const SurfaceInteraction& si,
		const Vec3f& wo,
		uint32_t active) const
{
	return { Eval(ctx, si, wo, active), Pdf(ctx, si, wo, active) };
}

float CPUBSDF::EvalNullTransmission(const SurfaceInteraction&, uint32_t) const
{
	return 0.f;
}

CPU_NAMESPACE_CLOSE_SCOPE
