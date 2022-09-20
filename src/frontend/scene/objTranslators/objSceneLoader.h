#ifndef OBJSCENE_LOADER_H
#define OBJSCENE_LOADER_H

#include<vector>

#include <tiny_obj_loader.h>

#include "../../spindulysFrontend.h"

#include "../scene.h"
#include "../sceneLoader.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class ObjSceneLoader final : public SceneLoader
{
	public:
		ObjSceneLoader(Scene* scene) : SceneLoader(scene) { };
		~ObjSceneLoader() = default;

		virtual bool LoadScene(const std::string& filepath) override;

	private:
		void LoadMesh(const std::vector<Vec3f>& vertices, tinyobj::shape_t& shape);
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // OBJSCENE_LOADER_H
