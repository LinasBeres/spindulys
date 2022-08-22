#ifndef CPU_QUADMESH_H
#define CPU_QUADMESH_H

#include <geometry/quadmesh.h>

#include "../spindulysBackendCPU.h"

#include "cpuGeometry.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUQuadMesh final : public CPUGeometry, public QuadMesh
{
	public:
		CPUQuadMesh();
		CPUQuadMesh(const std::string& primName,
				const AffineSpace3f& transform,
				const Col3f& displayColor,
				const pxr::VtArray<pxr::GfVec3f>& points,
				const pxr::VtArray<int>& indices);

		virtual bool CreatePrototype(const RTCDevice& device) override;

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_QUADMESH_H
