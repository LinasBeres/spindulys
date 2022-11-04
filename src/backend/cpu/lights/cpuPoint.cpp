#include "cpuPoint.h"

#include "../utils/interaction.h"
#include "../utils/records.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

Col3f CPUPointLight::Eval(const SurfaceInteraction&, uint32_t) const
{
	return zero;
}

std::pair<DirectionSample, Col3f>
CPUPointLight::SampleDirection(const Interaction& it, const Vec2f&, uint32_t active) const
{
	const Vec3f direction = GetPosition() - it.p;
	const float dist2 = dot(direction, direction);
	const float invDist = rsqrt(dist2);

	DirectionSample ds(
	/* Position: */         GetPosition(),
	/* Surface Normal: */   zero,
	/* Surface UV: */       zero,
	/* Time Value: */       it.time,
	/* PDF: */              1.f,
	/* Delta: */            true,
	/* Direction: */        invDist * direction,
	/* Distance: */         sqrt(dist2),
	/* Associated light: */ this
	);

	const Col3f color = (active ? GetIntensity() : zero) * sqr(invDist);

	return { ds, color };
}

float CPUPointLight::PdfDirection(const Interaction&, const DirectionSample&, uint32_t) const
{
	return zero;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
