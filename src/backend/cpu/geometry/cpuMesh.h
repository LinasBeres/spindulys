#ifndef CPU_MESH_H
#define CPU_MESH_H

#include <geometry/mesh.h>

#include "../spindulysBackendCPU.h"

#include "cpuGeometry.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUMesh final : public CPUGeometry, public Mesh
{
	public:
		CPUMesh();
		CPUMesh(Mesh* mesh);

		virtual bool CreatePrototype(const RTCDevice& device) override;

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_MESH_H
