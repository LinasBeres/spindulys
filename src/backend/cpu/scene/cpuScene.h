#ifndef CPU_SCENE_H
#define CPU_SCENE_H

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <scene/scene.h>

#include "../spindulysBackendCPU.h"

#include "../geometry/cpuGeometry.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPUScene final : public Scene
{
	public:
		CPUScene();
		~CPUScene() = default;

		virtual void CommitScene() override { rtcCommitScene(_scene); }
		virtual bool CreateGeomerty(Geometry* geom) override;
		bool CommitGeometry(CPUGeometry* geometry);

		RTCScene GetScene() { return _scene; }

		virtual void ResetScene() override;

	private:
		RTCDevice _device = nullptr;
		RTCScene _scene = nullptr; // Contains the instanced (single or not) geometry objects. This is the scene we are tracing against.
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_SCENE_H
