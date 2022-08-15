#ifndef CPU_BSDF_H
#define CPU_BSDF_H

#include <string>

#include <spindulys/mathHelper.h>
#include <spindulys/math/vec3.h>
#include <spindulys/sampler.h>

#include <utils/helperStructs.h>

#include "../spindulysBackendCPU.h"



BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class BSDF
{
	public:
		BSDF() = default;

		virtual Vec3f Evaluate(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) = 0;
		virtual Vec3f Sample(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) = 0;
		virtual float Pdf(PixelSample& pixelSample,
				ShadingPoint& shadingPoint,
				BSDFSample& bsdfSample) = 0;

	protected:
		std::string _name;
	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_BSDF_H
