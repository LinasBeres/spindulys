#ifndef CPU_SCENE_H
#define CPU_SCENE_H

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <scene/scene.h>

#include "../spindulysBackendCPU.h"

#include "../geometry/cpuGeometry.h"

#include "../lights/cpuLight.h"

#include "../utils/ray.h"
#include "../utils/interaction.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUScene final : public Scene
{
	public:
		CPUScene();
		~CPUScene();

		virtual void CommitScene() override { rtcCommitScene(_scene); }
		virtual bool CreateGeomerty(Geometry* geom) override;
		bool CommitGeometry(CPUGeometry* geometry);

		virtual bool CreateLights(Light* light) override;

		RTCScene GetScene() { return _scene; }

		virtual void ResetScene() override;

		// Get Methods
		const CPUGeometry* GetGeometery(unsigned int geomInstanceID) const { return m_sceneGeometry.at(geomInstanceID).get(); }
		const CPULight*    GetEnvironment()                          const { return m_environment.get(); }

		SurfaceInteraction RayIntersect(const Ray& ray) const;

		const CPULight* LightHit(const SurfaceInteraction& si, uint32_t active = true) const;

	private:
		RTCDevice _device = nullptr;
		RTCScene _scene = nullptr; // Contains the instanced (single or not) geometry objects. This is the scene we are tracing against.

		std::unordered_map<unsigned int, std::unique_ptr<CPUGeometry>> m_sceneGeometry;

		std::vector<std::unique_ptr<CPULight>> m_lights;

		std::unique_ptr<CPULight> m_environment;
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_SCENE_H
