#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <memory>

#include "../spindulysBase.h"

#include "../camera/camera.h"
#include "../geometry/geometry.h"
#include "../lights/light.h"

BASE_NAMESPACE_OPEN_SCOPE

class Scene
{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		virtual void CommitScene() = 0;
		virtual bool CreateGeomerty(Geometry* geom) = 0;

		virtual bool CreateLight(Light* light) = 0;
		virtual void CreateDefaultLight() = 0;

		virtual int NumLights() const = 0;

		void AddFilePath(const std::string& filepath) { m_filepaths.emplace_back(filepath); }
		const std::vector<std::string>& GetFilePaths() const { return m_filepaths; }

		bool SetSceneCamera(size_t cameraIndex) { return cameraIndex != std::exchange(m_mainCamera, cameraIndex); }
		const Camera& GetSceneCamera() const { return *(m_cameras[m_mainCamera].get()); }
		Camera& UpdateSceneCamera() { return *(m_cameras[m_mainCamera].get()); }

		const std::vector<std::string> GetSceneCameras() const;
		bool CreateDefaultCamera();
		void AddCamera(Camera* camera) { m_cameras.emplace_back(camera); }

		void SetSceneDirty() { m_update = true; }
		bool SceneDirty() const { return m_update; }

		virtual void ResetScene();

	protected:
		std::vector<std::string> m_filepaths;

		size_t m_mainCamera = 0;
		std::vector<std::unique_ptr<Camera>> m_cameras;

		bool m_update = false;

		mutable std::mutex m_sceneMutex;
	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // SCENE_H
