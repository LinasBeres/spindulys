#ifndef CPU_ORENNAYAR_H
#define CPU_ORENNAYAR_H

#include "../spindulysBackendCPU.h"

#include "bsdf.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class OrenNayar final : public BSDF
{
	public:
		OrenNayar();

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
		float _roughness;
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_ORENNAYAR_H
