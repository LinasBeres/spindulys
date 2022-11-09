#ifndef CPU_AREA_LIGHT_H
#define CPU_AREA_LIGHT_H

#include <lights/area.h>

#include "../spindulysBackendCPU.h"

#include "cpuLight.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUAreaLight final : public CPULight, public AreaLight
{
	public:
		CPUAreaLight(const Col3f& radiance = Col3f(one))
			: AreaLight(radiance) { }

		virtual Col3f Eval(const SurfaceInteraction& si, uint32_t active) const override;

		virtual std::pair<DirectionSample, Col3f> SampleDirection(
				const Interaction& it, const Vec2f& sample,
				uint32_t active) const override;

		virtual float PdfDirection(const Interaction& it, const DirectionSample& ds,
				uint32_t active) const override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_AREA_LIGHT_H
