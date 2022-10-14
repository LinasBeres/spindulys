#ifndef CPU_DIRECT_H
#define CPU_DIRECT_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"

#include "../bsdf/lambert.h"
#include "../bsdf/orennayar.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class Direct final : public Integrator
{
	public:
		Direct();

		virtual void GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				RenderManager::Buffers buffers,
				const RenderManager::RenderGlobals& renderGlobals) override;

		std::pair<Col3f, float> Sample(const CPUScene* scene, PixelSample* pixelSample,
				const Ray& ray, Col3f* /* aovs */) const;

		// TODO: To remove once we have a proper material system.
		Lambert diffuseMat;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIRECT_H
