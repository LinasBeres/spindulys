#ifndef CPU_SCENE_H
#define CPU_SCENE_H

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <scene/scene.h>


#include "../spindulysCPU.h"

#include "../geometry/cpuGeometry.h"

#include "../lights/cpuLight.h"
#include "../lights/cpuConstant.h"

#include "../bsdf/cpuBSDF.h"

#include "../utils/ray.h"
#include "../utils/interaction.h"
#include "../utils/records.h"

CPU_NAMESPACE_OPEN_SCOPE

class CPUScene final : public Scene
{
	public:
		CPUScene();
		~CPUScene();

		virtual void CommitScene() override { rtcCommitScene(m_scene); }
		virtual bool CreateGeomerty(Geometry* geom) override;
		bool CommitGeometry(CPUGeometry* geometry);

		virtual bool CreateLight(Light* light) override;
		virtual void CreateDefaultLight() override;

		virtual int NumLights() const override { return m_lights.size(); }

		RTCScene GetScene() { return m_scene; }

		virtual void ResetScene() override;

		// Get Methods
		const CPUGeometry* GetGeometery(unsigned int geomInstanceID) const { return m_sceneGeometry.at(geomInstanceID).get(); }
		const CPULight*    GetLight(uint32_t lightIndex)             const { return m_lights[lightIndex].get(); }
		const CPULight*    GetEnvironment()                          const { return m_environment.get(); }

		bool               RayTest(const Ray& ray) const;
		SurfaceInteraction RayIntersect(const Ray& ray) const;

		const CPULight* LightHit(const SurfaceInteraction& si, uint32_t active = true) const;

		std::pair<DirectionSample, Col3f>
		SampleLightDirection(const Interaction& ref, const Vec2f& sample, bool testVisibility, uint32_t active) const;

		float PdfLightDirection(const Interaction& ref, const DirectionSample& ds, uint32_t active) const;

		std::tuple<uint32_t, float, float>
		SampleLight(float indexSample, uint32_t active) const;

		float PdfLight() const { return m_lightPMF; }

	private:
		RTCDevice m_device = nullptr;
		RTCScene m_scene = nullptr; // Contains the instanced (single or not) geometry objects. This is the scene we are tracing against.

		std::unordered_map<unsigned int, std::unique_ptr<CPUGeometry>> m_sceneGeometry;

		std::vector<std::unique_ptr<CPULight>> m_lights;

		std::unique_ptr<CPULight> m_environment;

		float m_lightPMF = m_lights.empty() ? 0.f : (1.f / m_lights.size());
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_SCENE_H
