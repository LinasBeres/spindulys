#ifndef CPU_DIFFUSE_H
#define CPU_DIFFUSE_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class DiffuseIntegrator final : public Integrator
{
	public:
		DiffuseIntegrator();

		virtual Col3f GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				const RenderManager::RenderGlobals& renderGlobals) override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIFFUSE_H
