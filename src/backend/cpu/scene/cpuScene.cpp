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
	const float ray_maxt = ray.tfar;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	rtcIntersect1(_scene, &context, RTCRayHit_(ray));

	if (ray.tfar != ray_maxt)
	{
		uint32_t shape_index = ray.geomID;
		uint32_t prim_index  = ray.primID;

		// We get level 0 because we only support one level of instancing
		uint32_t inst_index = ray.instID;

		// If the hit is not on an instance
		bool hit_instance = inst_index != RTC_INVALID_GEOMETRY_ID;
		uint32_t index = hit_instance ? inst_index : shape_index;

		const Geometry& geom = GetGeometery(index);

		std::cerr << "Hit: " << geom.GetName() << " with prim id: " << prim_index << "\n";
	}
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
