#ifndef CPU_TRIANGLEMESH_H
#define CPU_TRIANGLEMESH_H

#include <geometry/trianglemesh.h>

#include "../spindulysBackendCPU.h"

#include "cpuGeometry.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUTriangleMesh final : public CPUGeometry, public TriangleMesh
{
	public:
		CPUTriangleMesh();
		CPUTriangleMesh(const std::string& primName,
				const LinearSpace3f& transform,
				const Col3f& displayColor,
				const pxr::VtArray<pxr::GfVec3f>& points,
				const pxr::VtArray<int>& indices);

		virtual bool CreatePrototype(const RTCDevice& device) override;

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_TRIANGLEMESH_H
