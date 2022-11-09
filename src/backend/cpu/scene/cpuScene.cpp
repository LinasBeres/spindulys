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

bool CPUScene::RayTest(const Ray& ray) const
{
	const float rayMaxT = ray.tfar;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	PreliminaryIntersection pi;

	rtcIntersect1(_scene, &context, RTCRayHit_(ray));

	return ray.tfar != rayMaxT;

}

SurfaceInteraction CPUScene::RayIntersect(const Ray& ray) const
{
	const float rayMaxT = ray.tfar;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	PreliminaryIntersection pi;

	rtcIntersect1(_scene, &context, RTCRayHit_(ray));

	if (ray.tfar != rayMaxT)
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


std::pair<DirectionSample, Col3f>
CPUScene::SampleLightDirection(const Interaction& ref, const Vec2f& sample_, bool testVisibility, uint32_t active) const
{
	Vec2f sample(sample_);
	DirectionSample ds;
	Col3f color(zero);

	const size_t lightCount = m_lights.size();
	if (lightCount > 1)
	{
		// Randomly pick an emitter
		auto [index, lightWeight, resampledX] = SampleLight(sample.x, active);
		sample.x = resampledX;

		// Sample a direction towards the emitter
		std::tie(ds, color) = m_lights[index]->SampleDirection(ref, sample, active);

		// Account for the discrete probability of sampling this emitter
		ds.pdf *= PdfLight();
		color *= lightWeight;

		active &= ds.pdf != 0.f;

		// Mark occluded samles as invalid if requested by the user
		if (testVisibility && active)
		{
			if (RayTest(ref.SpawnRayTo(ds.p)))
			{
				color = zero;
				ds.pdf = 0.f;
			}
		}
	}
	else if (lightCount == 1)
	{
		std::tie(ds, color) = m_lights[0]->SampleDirection(ref, sample, active);
		active &= ds.pdf != 0.f;

		if (testVisibility && active)
		{
			if (RayTest(ref.SpawnRayTo(ds.p)))
			{
				color = zero;
				ds.pdf = 0.f;
			}
		}
	}

	return { ds, color };
}

std::tuple<uint32_t, float, float>
CPUScene::SampleLight(float indexSample, uint32_t active) const
{
	if (unlikely(m_lights.size() < 2))
	{
		if (m_lights.size() == 1)
			return { size_t(0), 1.f, indexSample };
		else
			return { size_t(-1), 0.f, indexSample};
	}

	const uint32_t lightCount = (uint32_t) m_lights.size();
	const float lightCountf = (float) lightCount;
	const float indexSampleScaled = indexSample * lightCountf;

	const uint32_t index = min(uint32_t(indexSampleScaled), lightCount - 1u);

	return { index, lightCountf, indexSampleScaled - float(index) };

}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
