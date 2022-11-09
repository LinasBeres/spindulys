#ifndef CPU_MESH_H
#define CPU_MESH_H

#include <geometry/mesh.h>

#include "../spindulysCPU.h"

#include "cpuGeometry.h"


CPU_NAMESPACE_OPEN_SCOPE

class CPUMesh final : public CPUGeometry, public Mesh
{
	public:
		CPUMesh();
		CPUMesh(Mesh* mesh);

		virtual bool CreatePrototype(const RTCDevice& device) override;

		virtual SurfaceInteraction ComputeSurfaceInteraction(const Ray& ray,
				const PreliminaryIntersection& pi,
				uint32_t rayFlags = (uint32_t) RayFlags::All,
				uint32_t recursionDepth = 0, bool active = true) const override;

	private:
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_MESH_H
