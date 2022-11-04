#ifndef CPU_LAMBERT_H
#define CPU_LAMBERT_H

#include "../spindulysBackendCPU.h"

#include "../utils/interaction.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class Lambert
{
	public:
		Lambert();

		Vec3f Evaluate(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample);
		Vec3f Evaluate2(PixelSample& pixelSample,
				SurfaceInteraction& shadingPoint,
				BSDFSample& bsdfSample);

		Vec3f Sample(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample);
		Vec3f Sample2(PixelSample& pixelSample,
				SurfaceInteraction& shadingPoint,
				BSDFSample& bsdfSample);

		float Pdf(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample);

		float Pdf2(PixelSample& pixelSample,
				SurfaceInteraction& shadingPoint,
				BSDFSample& bsdfSample);
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_LAMBERT_H
