#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H


#include "../spindulysFrontend.h"

#include "scene.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class SceneLoader
{
	public:
		SceneLoader(Scene* scene) : scene(scene) { };
		~SceneLoader() = default;

		virtual bool LoadScene(const std::string& filepath) = 0;

	protected:
		Scene* scene = nullptr;
	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // SCENE_LOADER_H
