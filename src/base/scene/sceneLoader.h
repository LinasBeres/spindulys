#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H


#include "../spindulysBase.h"

#include "scene.h"

BASE_NAMESPACE_OPEN_SCOPE

class SceneLoader
{
	public:
		SceneLoader(Scene* scene) : m_scene(scene) { };
		~SceneLoader() = default;

		virtual bool LoadScene(const std::string& filepath) = 0;

	protected:
		Scene* m_scene = nullptr;
	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // SCENE_LOADER_H
