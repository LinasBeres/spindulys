#ifndef CPU_DIRECT_H
#define CPU_DIRECT_H

#include "../spindulysBackendCPU.h"

#include "integrator.h"

#include "../bsdf/lambert.h"
#include "../bsdf/orennayar.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class Direct
{
	public:
		Direct();

		std::pair<Col3f, float> Sample(const CPUScene* scene, PixelSample& pixelSample,
				const Ray& ray, Col3f* /* aovs */);

		// TODO: To remove once we have a proper material system.
		Lambert diffuseMat;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIRECT_H
