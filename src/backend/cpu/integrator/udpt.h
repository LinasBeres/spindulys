#ifndef CPU_UPDT_H
#define CPU_UPDT_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"

#include "../bsdf/lambert.h"
#include "../bsdf/orennayar.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class UDPTIntegrator final : public Integrator
{
	public:
		UDPTIntegrator();

		virtual Col3f GetPixelColor(Ray& ray,
				PixelSample& pixelSample,
				CPUScene* scene,
				const RenderManager::RenderGlobals& renderGlobals) override;

		// TODO: To remove once we have a proper material system.
		Lambert diffuseMat;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_UPDT_H
