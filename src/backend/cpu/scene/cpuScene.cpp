#include "cpuScene.h"

#include "../geometry/cpuMesh.h"
#include "../geometry/cpuCurve.h"

#include "../lights/cpuConstant.h"

#include <spindulys/math/linearspace3.h>


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUScene::CPUScene()
{
	_device = rtcNewDevice("");
	_scene = rtcNewScene(_device);
  rtcSetSceneFlags(_scene, RTC_SCENE_FLAG_DYNAMIC);

	m_environment = std::make_unique<CPUConstantLight>(10.f, Vec3f(zero), 1.f, Col3f(0.7, 0.8, 0.9));
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
	m_sceneGeometry[geometry->GetGeomInstanceID()] = std::unique_ptr<CPUGeometry>(geometry);
	_sceneMutex.unlock();

	return true;
}

bool CPUScene::CreateLights(Light* light)
{

	return true;
}

void CPUScene::ResetScene()
{
	// Reset embree render scene.
	rtcReleaseScene(_scene);
	_scene = rtcNewScene(_device);
  rtcSetSceneFlags(_scene, RTC_SCENE_FLAG_DYNAMIC);

	m_sceneGeometry.clear();
	m_lights.clear();
	m_environment.reset(nullptr);
	m_environment = std::make_unique<CPUConstantLight>(10.f, Vec3f(zero), 1.f, Col3f(0.7, 0.8, 0.9));

	// Reset Parent scene stuff
	Scene::ResetScene();
}


// TODO: This should return a surface interaction
SurfaceInteraction CPUScene::RayIntersect(const Ray& ray) const
{
	const float ray_maxt = ray.tfar;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	PreliminaryIntersection pi;

	rtcIntersect1(_scene, &context, RTCRayHit_(ray));

	if (ray.tfar != ray_maxt)
	{
		uint32_t shapeIndex = ray.geomID;
		uint32_t primIndex  = ray.primID;

		// We get level 0 because we only support one level of instancing
		uint32_t instIndex = ray.instID;

		// If the hit is not on an instance
		bool hit_instance = instIndex != RTC_INVALID_GEOMETRY_ID;
		uint32_t index = hit_instance ? instIndex : shapeIndex;

		const CPUGeometry* geom = GetGeometery(index);

		// Initialise preliminary intersection
		pi.t = ray.tfar;
		pi.instance = geom;
		pi.shape = geom;

		pi.shapeIndex = shapeIndex;
		pi.primIndex = primIndex;
		pi.primUV = Vec2f(ray.u, ray.v);
	}

	return pi.ComputeSurfaceInteraction(ray);
}


const CPULight* CPUScene::LightHit(const SurfaceInteraction& si, uint32_t active) const
{
	// TODO: Return shape light if hit that.
	return si.IsValid() ? nullptr : GetEnvironment();
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
