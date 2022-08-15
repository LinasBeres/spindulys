#ifndef CPU_POSITION_H
#define CPU_POSITION_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class PositionIntegrator final : public Integrator
{
	public:
		PositionIntegrator();

		virtual Col3f GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				const RenderManager::RenderGlobals& renderGlobals) override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_POSITION_H
