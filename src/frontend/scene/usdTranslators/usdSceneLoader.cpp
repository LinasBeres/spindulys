#include "usdSceneLoader.h"

#include "usdCameraTranslator.h"
#include "usdMeshTranslator.h"
#include "usdBasisCurveTranslator.h"

#include "../../camera/camera.h"
#include "../../geometry/mesh.h"
#include "../../geometry/curve.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

bool UsdSceneLoader::LoadScene(const std::string& filepath)
{
	// We assume that the incoming scene is a valid usd scene.

	const pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(filepath);
	if (!stage)
		return false;

	LoadPrims(stage, pxr::SdfPath("/"));

	if (scene->GetSceneCameras().empty())
		scene->CreateDefaultCamera();

	scene->AddFilePath(filepath);
	scene->CommitScene();

	return true;
}

bool UsdSceneLoader::LoadPrims(const pxr::UsdStagePtr& stage, const pxr::SdfPath& primPath)
{
	const pxr::UsdPrim basePrim = stage->GetPrimAtPath(primPath);

	for(const pxr::UsdPrim& prim: basePrim.GetChildren())
	{
		if (prim.GetTypeName() == "Camera")
		{
			if (UsdCameraTranslator trans; Camera* camera = (Camera*)trans.GetObjectFromPrim(prim))
				scene->AddCamera(camera);
		}
		else if (prim.GetTypeName() == "Mesh")
		{
			if (UsdMeshTranslator trans; Mesh* mesh = (Mesh*)trans.GetObjectFromPrim(prim))
				scene->CreateGeomerty(mesh);
		}
		else if (prim.GetTypeName() == "BasisCurves")
		{
			if (UsdBasisCurveTranslator trans; Curve* curve = (Curve*)trans.GetObjectFromPrim(prim))
				scene->CreateGeomerty(curve);
		}

		if (prim.GetChildren())
			LoadPrims(stage, prim.GetPath());
	}

	return true;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE