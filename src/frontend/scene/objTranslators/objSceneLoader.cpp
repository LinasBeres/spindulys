#include "objSceneLoader.h"

#include "../../geometry/mesh.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

bool ObjSceneLoader::LoadScene(const std::string& filepath)
{
	// We assume that the incoming file is a valid obj file

	scene->AddFilePath(filepath);
	scene->CommitScene();

	return true;
}

bool ObjSceneLoader::LoadMesh()
{

	return true;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
