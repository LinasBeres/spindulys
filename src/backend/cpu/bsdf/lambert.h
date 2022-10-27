#ifndef CPU_LAMBERT_H
#define CPU_LAMBERT_H

#include "bsdf.h"

#include "../spindulysBackendCPU.h"

#include "../utils/interaction.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class Lambert final : public BSDF
{
	public:
		Lambert();

		virtual Vec3f Evaluate(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) override;
		Vec3f Evaluate2(PixelSample& pixelSample,
				SurfaceInteraction& shadingPoint,
				BSDFSample& bsdfSample);

		virtual Vec3f Sample(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) override;
		Vec3f Sample2(PixelSample& pixelSample,
				SurfaceInteraction& shadingPoint,
				BSDFSample& bsdfSample);

		virtual float Pdf(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) override;

		float Pdf2(PixelSample& pixelSample,
				SurfaceInteraction& shadingPoint,
				BSDFSample& bsdfSample);
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_LAMBERT_H
