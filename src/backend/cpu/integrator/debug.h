#ifndef CPU_DEBUG_H
#define CPU_DEBUG_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class DebugIntegrator final : public Integrator
{
	public:
		DebugIntegrator();

		virtual Col3f GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				const RenderManager::RenderGlobals& renderGlobals) override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // DEBUG_H
