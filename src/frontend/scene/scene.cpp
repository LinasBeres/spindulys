#include "scene.h"

#include <vector>

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include "../geometry/mesh.h"
#include "../geometry/curve.h"

#include "usdTranslators/usdCameraTranslator.h"
#include "usdTranslators/usdMeshTranslator.h"
#include "usdTranslators/usdBasisCurveTranslator.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

bool Scene::LoadScene(const std::string& filepath)
{
	if (filepath.empty())
		return false;

	if (!((std::size_t(filepath.rfind(std::string(".usd")) != std::string::npos) ||
					std::size_t(filepath.rfind(std::string(".usda")) != std::string::npos) ||
					std::size_t(filepath.rfind(std::string(".usdc")) != std::string::npos) ||
					std::size_t(filepath.rfind(std::string(".usdz")) != std::string::npos)) ? true : false))
	{
		std::cerr << "ERROR - The following file is not an USD scene: " << filepath << std::endl;

		return false;
	}

	const pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(filepath);
	if (!stage)
		return false;

	LoadMeshGeometry(stage);

	// We need to have at least one camera...
	if (_cameras.empty())
		_cameras.emplace_back(new Camera("SpindulysDefaultCamera"));

	return true;
}

void Scene::LoadPrims(const pxr::UsdStagePtr& stage, const pxr::SdfPath& primPath)
{
	const pxr::UsdPrim basePrim = stage->GetPrimAtPath(primPath);

	for(const pxr::UsdPrim& prim: basePrim.GetChildren())
	{
		if (prim.GetTypeName() == "Camera")
		{
			if (UsdCameraTranslator trans; Camera* camera = (Camera*)trans.GetObjectFromPrim(prim))
				_cameras.emplace_back(camera);
		}
		else if (prim.GetTypeName() == "Mesh")
		{
			if (UsdMeshTranslator trans; Mesh* mesh = (Mesh*)trans.GetObjectFromPrim(prim))
				CreateGeomerty(mesh);
		}
		else if (prim.GetTypeName() == "BasisCurves")
		{
			if (UsdBasisCurveTranslator trans; Curve* curve = (Curve*)trans.GetObjectFromPrim(prim))
				CreateGeomerty(curve);
		}

		if (prim.GetChildren())
			LoadPrims(stage, prim.GetPath());
	}

}

bool Scene::LoadMeshGeometry(const pxr::UsdStagePtr& stage)
{
	std::vector<pxr::UsdPrim> meshPrims;

	LoadPrims(stage, pxr::SdfPath("/"));

	CommitScene();

	return true;
}

const std::vector<std::string> Scene::GetSceneCameras() const
{
	std::vector<std::string> cameras;
	for (const auto& camera : _cameras)
		cameras.emplace_back(camera->GetName());

	return cameras;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE

