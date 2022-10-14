#include "cpuScene.h"

#include "../geometry/cpuMesh.h"
#include "../geometry/cpuCurve.h"

#include <spindulys/math/linearspace3.h>


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUScene::CPUScene()
{
	_device = rtcNewDevice("");
	_scene = rtcNewScene(_device);
  rtcSetSceneFlags(_scene, RTC_SCENE_FLAG_DYNAMIC);
}

CPUScene::~CPUScene()
{
	rtcReleaseScene(_scene);
	rtcReleaseDevice(_device);
}

bool CPUScene::CreateGeomerty(Geometry* geom)
{
	bool success = true;

	switch(geom->GetGeometryType())
	{
		case Geometry::Mesh:
		{
			CPUMesh* mesh(new CPUMesh(dynamic_cast<Mesh*>(geom)));
			success &= CommitGeometry(mesh);
			break;
		}
		case Geometry::Curve:
		{
			CPUCurve* curve(new CPUCurve(dynamic_cast<Curve*>(geom)));
			success &= CommitGeometry(curve);
			break;
		}
		default:
			break;
	}

	delete geom;

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

void CPUScene::ResetScene()
{
	// Reset embree render scene.
	rtcReleaseScene(_scene);
	_scene = rtcNewScene(_device);
  rtcSetSceneFlags(_scene, RTC_SCENE_FLAG_DYNAMIC);

	// Reset Parent scene stuff
	Scene::ResetScene();
}


// TODO: This should return a surface interaction
void CPUScene::RayIntersect(const Ray& ray) const
{
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	rtcIntersect1(_scene, &context, RTCRayHit_(ray));
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
