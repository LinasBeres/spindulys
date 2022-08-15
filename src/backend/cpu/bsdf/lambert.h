#ifndef CPU_LAMBERT_H
#define CPU_LAMBERT_H

#include "bsdf.h"

#include "../spindulysBackendCPU.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class Lambert final : public BSDF
{
	public:
		Lambert();

		virtual Vec3f Evaluate(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) override;
		virtual Vec3f Sample(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) override;
		virtual float Pdf(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) override;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_LAMBERT_H
