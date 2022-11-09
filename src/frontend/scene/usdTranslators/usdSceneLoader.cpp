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
	FRONTEND_TRACE();
	// We assume that the incoming scene is a valid usd scene.

	FRONTEND_BEGIN("LOAD USD STAGE");
	const pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(filepath);
	if (!stage)
		return false;
	FRONTEND_END("LOAD USD STAGE");

	LoadPrims(stage, pxr::SdfPath("/"));

	scene->AddFilePath(filepath);
	FRONTEND_BEGIN("COMMIT SCENE");
	scene->CommitScene();
	FRONTEND_END("COMMIT SCENE");

	return true;
}

bool UsdSceneLoader::LoadPrims(const pxr::UsdStagePtr& stage, const pxr::SdfPath& primPath)
{
	FRONTEND_TRACE();
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
