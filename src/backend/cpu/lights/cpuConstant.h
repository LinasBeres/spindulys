#ifndef CPU_CONSTANT_LIGHT_H
#define CPU_CONSTANT_LIGHT_H

#include <lights/constant.h>

#include "../spindulysBackendCPU.h"

#include "cpuLight.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUConstantLight final : public CPULight, public ConstantLight
{
	public:
		CPUConstantLight();

		virtual Col3f Eval(const SurfaceInteraction& si, uint32_t active) const override;

		virtual std::pair<DirectionSample, Col3f> SampleDirection(
				const Interaction& it, const Vec2f& sample,
				uint32_t active) const override;

		virtual float PdfDirection(const Interaction& it, const DirectionSample& ds,
				uint32_t active) const override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_CONSTANT_LIGHT_H
