#include "cpuScene.h"

#include "../geometry/cpuMesh.h"
#include "../geometry/cpuCurve.h"

#include "../lights/cpuConstant.h"
#include "../lights/cpuPoint.h"

#include <spindulys/math/linearspace3.h>


CPU_NAMESPACE_OPEN_SCOPE

CPUScene::CPUScene()
{
	m_device = rtcNewDevice("");
	m_scene = rtcNewScene(m_device);
  rtcSetSceneFlags(m_scene, RTC_SCENE_FLAG_DYNAMIC);
}

CPUScene::~CPUScene()
{
	rtcReleaseScene(m_scene);
	rtcReleaseDevice(m_device);
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
	if(!geometry->Create(m_device, m_scene))
		return false;

	m_sceneMutex.lock();
	m_sceneGeometry[geometry->GetGeomInstanceID()] = std::unique_ptr<CPUGeometry>(geometry);
	m_sceneMutex.unlock();

	return true;
}

bool CPUScene::CreateLight(Light* light)
{
	if (!light)
		return false;

	m_sceneMutex.lock();
	// m_lights.emplace_back(std::unique_ptr<CPULight>(light));
	m_lightPMF = m_lights.empty() ? 0.f : (1.f / m_lights.size());
	m_sceneMutex.unlock();

	return true;
}

void CPUScene::CreateDefaultLight()
{
	// Can only have one enivronment light.
	assert(m_environment == nullptr);

	m_environment = std::make_unique<CPUConstantLight>();

	// Reduce the light level of the environment light
	if (auto constantLight = dynamic_cast<CPUConstantLight*>(m_environment.get()))
		constantLight->SetRadiance(Col3f(0.1f));

	// TODO: Fix this.
	// m_lights.emplace_back(m_environment);
	// m_lightPMF = m_lights.empty() ? 0.f : (1.f / m_lights.size());
}

void CPUScene::ResetScene()
{
	// Reset embree render scene.
	rtcReleaseScene(m_scene);
	m_scene = rtcNewScene(m_device);
  rtcSetSceneFlags(m_scene, RTC_SCENE_FLAG_DYNAMIC);

	m_sceneGeometry.clear();
	m_lights.clear();

	// TODO: Remove once scenes have lights and automatically create an environment light if no lights
	// CPUPointLight* l = new CPUPointLight(AffineSpace3f(one, Vec3f(-1.5f, 1.9f, -11.f)), Col3f(10.f, 14.f, 10.f));
	// m_lights.emplace_back(l);

	m_lightPMF = m_lights.empty() ? 0.f : (1.f / m_lights.size());

	// Reset Parent scene stuff
	Scene::ResetScene();
}

bool CPUScene::RayTest(const Ray& ray) const
{
	const float rayMaxT = ray.tfar;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	PreliminaryIntersection pi;

	rtcIntersect1(m_scene, &context, RTCRayHit_(ray));

	// Return true if we hit something
	return ray.tfar != rayMaxT;
}

SurfaceInteraction CPUScene::RayIntersect(const Ray& ray) const
{
	const float rayMaxT = ray.tfar;

	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	PreliminaryIntersection pi;

	rtcIntersect1(m_scene, &context, RTCRayHit_(ray));

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

CPU_NAMESPACE_CLOSE_SCOPE
