#ifndef CPU_INTGRATOR_H
#define CPU_INTGRATOR_H

#include <spindulys/math/col3.h>

#include <render/renderManager.h>

#include "../spindulysBackendCPU.h"

#include "../scene/cpuScene.h"
#include "../utils/ray.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class Integrator
{
	public:
		Integrator();

		virtual void GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				RenderManager::Buffers buffers,
				const RenderManager::RenderGlobals& renderGlobals);
		virtual ShadingPoint SetupShadingPoint(const CPUScene* scene, const Ray& ray);

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_INTGRATOR_H
