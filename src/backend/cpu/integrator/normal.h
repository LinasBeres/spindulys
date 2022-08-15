#ifndef CPU_NORMAL_H
#define CPU_NORMAL_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class NormalIntegrator final : public Integrator
{
	public:
		NormalIntegrator();

		virtual Col3f GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				const RenderManager::RenderGlobals& renderGlobals) override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_NORMAL_H
