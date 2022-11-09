#ifndef CPU_GEOMETRY_H
#define CPU_GEOMETRY_H

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <spindulys/math/vec3.h>
#include <spindulys/math/col3.h>

#include <geometry/geometry.h>

#include "../spindulysBackendCPU.h"

#include "../bsdf/bsdf.h"

#include "../utils/ray.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

struct SurfaceInteraction;
struct PreliminaryIntersection;

class CPUGeometry : virtual public Geometry
{
	public:
		CPUGeometry();
		virtual ~CPUGeometry();

		virtual bool Create(const RTCDevice& device, const RTCScene& topScene);
		virtual bool CreatePrototype(const RTCDevice& device) = 0;

		virtual SurfaceInteraction ComputeSurfaceInteraction(const Ray& ray,
				const PreliminaryIntersection& pi,
				uint32_t rayFlags = (uint32_t) RayFlags::All,
				uint32_t recursionDepth = 0, bool active = true) const = 0;

		void ComputeInstanceSurfaceInteraction(SurfaceInteraction& si, const Ray& ray) const;

		const BSDF* GetBSDF() const { return m_bsdf.get(); }

	protected:
		RTCScene _scene = nullptr;
		RTCGeometry _geom = nullptr;

	private:
		RTCGeometry _geomInstance = nullptr;

		std::unique_ptr<BSDF> m_bsdf;
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_GEOMETRY_H
