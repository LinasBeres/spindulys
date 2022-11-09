#include "cpuArea.h"

#include "../utils/interaction.h"
#include "../utils/records.h"

CPU_NAMESPACE_OPEN_SCOPE

Col3f CPUAreaLight::Eval(const SurfaceInteraction& si, uint32_t active) const
{
	return (active && si.wi.z > 0.f) ? GetRadiance() : Col3f(zero);
}

std::pair<DirectionSample, Col3f> CPUAreaLight::SampleDirection(
		const Interaction& it, const Vec2f& sample,
		uint32_t active) const
{
	// TODO:
	DirectionSample ds(zero, zero, zero, zero, zero, zero, zero, zero, nullptr);
	return { ds, zero };
}

float CPUAreaLight::PdfDirection(const Interaction& it, const DirectionSample& ds,
		uint32_t active) const
{
	// TODO:
	return zero;
}

CPU_NAMESPACE_CLOSE_SCOPE
