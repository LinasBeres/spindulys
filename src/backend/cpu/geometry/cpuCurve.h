#ifndef CPU_CURVE_H
#define CPU_CURVE_H

#include <geometry/curve.h>

#include "../spindulysBackendCPU.h"

#include "cpuGeometry.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUCurve final : public CPUGeometry, public Curve
{
	public:
		CPUCurve();
		CPUCurve(Curve* curve);

		virtual bool CreatePrototype(const RTCDevice& device) override;

		virtual SurfaceInteraction ComputeSurfaceInteraction(const Ray& ray,
				const PreliminaryIntersection& pi,
				uint32_t rayFlags = (uint32_t) RayFlags::All,
				uint32_t recursionDepth = 0, bool active = true) const override;

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_CURVE_H
