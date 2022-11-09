#include "cpuConstant.h"

#include <spindulys/math/warp.h>

#include "../utils/interaction.h"
#include "../utils/records.h"

CPU_NAMESPACE_OPEN_SCOPE

Col3f CPUConstantLight::Eval(const SurfaceInteraction&, uint32_t active) const
{
	return active ? GetRadiance() : Col3f(zero);
}

std::pair<DirectionSample, Col3f> CPUConstantLight::SampleDirection(
		const Interaction& it, const Vec2f& sample,
		uint32_t active) const
{
	const Vec3f d = square_to_uniform_sphere(sample);

	// Automatically enlarge the bounding sphere when it does not contain the reference point
	const float radius = max(m_radius, length(it.p - m_point));
	const float dist   = 2.f * radius;

	DirectionSample ds(
	/* Position: */   madd(dist, d, it.p),
	/* Normal: */     -d,
	/* UV: */         sample,
	/* Time value: */ it.time,
	/* pdf: */        square_to_uniform_sphere_pdf(),
	/* delta: */      false,
	/* direction: */  d,
	/* distance: */   dist,
	/* light: */      this
	);

	return { ds, GetRadiance() / ds.pdf };

}

float CPUConstantLight::PdfDirection(const Interaction& it, const DirectionSample& ds,
		uint32_t active) const
{
	return InvFourPi<float>;
}

CPU_NAMESPACE_CLOSE_SCOPE
