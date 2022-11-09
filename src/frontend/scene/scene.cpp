#include "scene.h"

#include <vector>

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include "../geometry/mesh.h"
#include "../geometry/curve.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

const std::vector<std::string> Scene::GetSceneCameras() const
{
	FRONTEND_TRACE();
	std::vector<std::string> cameras;
	for (const auto& camera : _cameras)
		cameras.emplace_back(camera->GetName());

	return cameras;
}

bool Scene::CreateDefaultCamera()
{
	FRONTEND_TRACE();
	for (const auto& camera : _cameras)
		if (camera->GetName() == "SpindulysDefaultCamera")
			return false;

	_cameras.emplace_back(new Camera("SpindulysDefaultCamera"));
	return true;
}

void Scene::ResetScene()
{
	FRONTEND_TRACE();
	_filepaths.clear();
	_mainCamera = 0;
	_cameras.clear();
}

FRONTEND_NAMESPACE_CLOSE_SCOPE

