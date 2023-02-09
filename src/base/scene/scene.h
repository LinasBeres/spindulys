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

		virtual bool CreateLights(Light* light) = 0;

		void AddFilePath(const std::string& filepath) { _filepaths.emplace_back(filepath); }
		const std::vector<std::string>& GetFilePaths() const { return _filepaths; }

		bool SetSceneCamera(size_t cameraIndex) { return cameraIndex != std::exchange(_mainCamera, cameraIndex); }
		const Camera& GetSceneCamera() const { return *(_cameras[_mainCamera].get()); }
		Camera& UpdateSceneCamera() { return *(_cameras[_mainCamera].get()); }

		const std::vector<std::string> GetSceneCameras() const;
		bool CreateDefaultCamera();
		void AddCamera(Camera* camera) { _cameras.emplace_back(camera); }

		void SetSceneDirty() { _update = true; }
		bool SceneDirty() const { return _update; }

		virtual void ResetScene();

	protected:
		std::vector<std::string> _filepaths;

		size_t _mainCamera = 0;
		std::vector<std::unique_ptr<Camera>> _cameras;

		bool _update = false;

		mutable std::mutex _sceneMutex;
	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // SCENE_H
