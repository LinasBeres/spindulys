#ifndef OCCLUSION_H
#define OCCLUSION_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class OcclusionIntegrator final : public Integrator
{
	public:
		OcclusionIntegrator();

		virtual Col3f GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				const RenderManager::RenderGlobals& renderGlobals) override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // OCCLUSION_H
