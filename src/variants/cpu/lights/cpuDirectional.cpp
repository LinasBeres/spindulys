#include "cpuDirectional.h"

#include "../utils/interaction.h"
#include "../utils/records.h"

CPU_NAMESPACE_OPEN_SCOPE

Col3f CPUDirectionalLight::Eval(const SurfaceInteraction& si, uint32_t active) const
{
	return Col3f(zero);
}

std::pair<DirectionSample, Col3f> CPUDirectionalLight::SampleDirection(
		const Interaction& it, const Vec2f& sample,
		uint32_t active) const
{
	// TODO:
	DirectionSample ds(zero, zero, zero, zero, zero, zero, zero, zero, nullptr);
	return { ds, zero };
}

float CPUDirectionalLight::PdfDirection(const Interaction& it, const DirectionSample& ds,
		uint32_t active) const
{
	return zero;
}

CPU_NAMESPACE_CLOSE_SCOPE
