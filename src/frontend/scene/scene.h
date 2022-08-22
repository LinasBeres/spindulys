#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <mutex>
#include <unordered_map>

#include "../spindulysFrontend.h"

#include "../geometry/geometry.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class Scene
{
	public:
		Scene() = default;
		virtual ~Scene() = default;

		bool LoadScene(const std::string& filepath);
		bool LoadMeshGeometry(const pxr::UsdStagePtr& stage);
		virtual void CommitScene() = 0;
		virtual bool CreateGeomerty(Geometry::GeometryTypes geometryType,
				const std::string& primName,
				const AffineSpace3f& affine,
				const Col3f& displayColor,
				const pxr::VtArray<pxr::GfVec3f>& points,
				const pxr::VtArray<int>& indices) = 0;

		const std::string& GetFilePath() const { return filepath; }

		const Geometry& GetGeometery(unsigned int geomInstanceID) const { return *(_sceneGeometry.at(geomInstanceID).get()); }

		bool SceneDirty() const { return update; }

	protected:
		std::string filepath;
		std::unordered_map<unsigned int, std::unique_ptr<Geometry>> _sceneGeometry;
		std::mutex _sceneMutex;

		bool update = false;
	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // SCENE_H
