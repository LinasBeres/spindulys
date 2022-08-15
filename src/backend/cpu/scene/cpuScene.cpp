#include "cpuScene.h"

#include "../geometry/cpuTrianglemesh.h"
#include "../geometry/cpuQuadmesh.h"

#include <spindulys/math/linearspace3.h>


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUScene::CPUScene()
{
	_device = rtcNewDevice("");
	_scene = rtcNewScene(_device);
}

bool CPUScene::CreateGeomerty(Geometry::GeometryTypes geometryType,
		const std::string& primName,
		const LinearSpace3f& transform,
		const Col3f& displayColor,
		const pxr::VtArray<pxr::GfVec3f>& points,
		const pxr::VtArray<int>& indices)
{
	bool success = true;

	if (geometryType == Geometry::GeometryTypes::TriangleMesh)
	{
		CPUTriangleMesh* triangleMesh(new CPUTriangleMesh(primName, transform, displayColor, points, indices));
		success = success && CommitGeometry(triangleMesh);
	}
	else if (geometryType == Geometry::GeometryTypes::QuadMesh)
	{
		CPUQuadMesh* quadMesh(new CPUQuadMesh(primName, transform, displayColor, points, indices));
		success = success && CommitGeometry(quadMesh);
	}

	return success;
}

bool CPUScene::CommitGeometry(CPUGeometry* geometry)
{
	if(!geometry->Create(_device, _scene))
		return false;

	_sceneMutex.lock();
	_sceneGeometry[geometry->GetGeomInstanceID()] = std::unique_ptr<Geometry>(geometry);
	_sceneMutex.unlock();

	return true;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
