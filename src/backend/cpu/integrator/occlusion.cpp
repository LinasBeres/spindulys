#include "occlusion.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

OcclusionIntegrator::OcclusionIntegrator()
{
}

Col3f OcclusionIntegrator::GetPixelColor(Ray& ray,
		PixelSample& pixelSample,
		CPUScene* scene,
		const RenderManager::RenderGlobals& renderGlobals)
{
	// TODO

	return Col3f(1.0f);
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
