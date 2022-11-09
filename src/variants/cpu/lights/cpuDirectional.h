#ifndef CPU_DIRECTIONAL_LIGHT_H
#define CPU_DIRECTIONAL_LIGHT_H

#include <lights/directional.h>

#include "../spindulysCPU.h"

#include "cpuLight.h"


CPU_NAMESPACE_OPEN_SCOPE

class CPUDirectionalLight final : public CPULight, public DirectionalLight
{
	public:
		CPUDirectionalLight(const Col3f& irradiance = Col3f(one))
			: DirectionalLight(irradiance) { }

		virtual Col3f Eval(const SurfaceInteraction& si, uint32_t active) const override;

		virtual std::pair<DirectionSample, Col3f> SampleDirection(
				const Interaction& it, const Vec2f& sample,
				uint32_t active) const override;

		virtual float PdfDirection(const Interaction& it, const DirectionSample& ds,
				uint32_t active) const override;
	private:
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIRECTIONAL_LIGHT_H
