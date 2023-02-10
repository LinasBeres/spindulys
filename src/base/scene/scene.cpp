#include "scene.h"

#include <vector>

#include "../geometry/mesh.h"
#include "../geometry/curve.h"

BASE_NAMESPACE_OPEN_SCOPE

const std::vector<std::string> Scene::GetSceneCameras() const
{
	BASE_TRACE();
	std::vector<std::string> cameras;
	for (const auto& camera : m_cameras)
		cameras.emplace_back(camera->GetName());

	return cameras;
}

bool Scene::CreateDefaultCamera()
{
	BASE_TRACE();
	for (const auto& camera : m_cameras)
		if (camera->GetName() == "SpindulysDefaultCamera")
			return false;

	m_cameras.emplace_back(new Camera("SpindulysDefaultCamera"));
	return true;
}

void Scene::ResetScene()
{
	BASE_TRACE();
	m_filepaths.clear();
	m_mainCamera = 0;
	m_cameras.clear();
}

BASE_NAMESPACE_CLOSE_SCOPE

