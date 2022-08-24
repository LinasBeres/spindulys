#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "../spindulysFrontend.h"

#include "../camera/camera.h"
#include "../geometry/geometry.h"

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xformCache.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/base/vt/array.h>

FRONTEND_NAMESPACE_OPEN_SCOPE

class Scene
{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		bool LoadScene(const std::string& filepath);
		bool LoadMeshGeometry(const pxr::UsdStagePtr& stage);
		void LoadPrims(const pxr::UsdStagePtr& stage, const pxr::SdfPath& primPath);
		virtual void CommitScene() = 0;
		virtual bool CreateGeomerty(Geometry::GeometryTypes geometryType,
				const std::string& primName,
				const AffineSpace3f& affine,
				const Col3f& displayColor,
				const pxr::VtArray<pxr::GfVec3f>& points,
				const pxr::VtArray<int>& indices) = 0;

		const std::string& GetFilePath() const { return filepath; }

		const Geometry& GetGeometery(unsigned int geomInstanceID) const { return *(_sceneGeometry.at(geomInstanceID).get()); }

		bool SetSceneCamera(size_t cameraIndex) { return cameraIndex != std::exchange(_mainCamera, cameraIndex); }
		const Camera& GetSceneCamera() const { return *(_cameras[_mainCamera].get()); }
		Camera& UpdateSceneCamera() { return *(_cameras[_mainCamera].get()); }

		const std::vector<std::string> GetSceneCameras() const;

		void SetSceneDirty() { update = true; }
		bool SceneDirty() const { return update; }

	protected:
		std::string filepath;

		size_t _mainCamera = 0;
		std::vector<std::unique_ptr<Camera>> _cameras;

		std::unordered_map<unsigned int, std::unique_ptr<Geometry>> _sceneGeometry;

		bool update = false;

		std::mutex _sceneMutex;
	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // SCENE_H
