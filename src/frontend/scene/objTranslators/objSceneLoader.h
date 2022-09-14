#ifndef OBJSCENE_LOADER_H
#define OBJSCENE_LOADER_H

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
		bool LoadMesh();
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // OBJSCENE_LOADER_H
