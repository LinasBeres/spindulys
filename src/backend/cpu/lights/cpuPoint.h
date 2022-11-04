#ifndef CPU_POINT_LIGHT_H
#define CPU_POINT_LIGHT_H

#include <lights/point.h>

#include "../spindulysBackendCPU.h"

#include "cpuLight.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUPointLight final : public CPULight, public PointLight
{
public:
	CPUPointLight(const AffineSpace3f& transform = AffineSpace3f(one, zero),
				  const Col3f& intensity = one)
		: PointLight(transform, intensity)
	{ }

	virtual Col3f Eval(const SurfaceInteraction& si, uint32_t active) const override;

	virtual std::pair<DirectionSample, Col3f>
	SampleDirection(const Interaction& it, const Vec2f& sample, uint32_t active) const override;

	virtual float
	PdfDirection(const Interaction& it, const DirectionSample& ds, uint32_t active) const override;

private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_Point_LIGHT_H
