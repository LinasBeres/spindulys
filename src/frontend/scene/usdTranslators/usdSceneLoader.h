#ifndef USDSCENE_LOADER_H
#define USDSCENE_LOADER_H

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>

#include "../../spindulysFrontend.h"

#include "../scene.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

class UsdSceneLoader
{
	public:
		UsdSceneLoader(Scene* scene) : scene(scene) { };
		~UsdSceneLoader() = default;

		bool LoadScene(const std::string& filepath);

	private:
		bool LoadPrims(const pxr::UsdStagePtr& stage, const pxr::SdfPath& primPath);

		Scene* scene = nullptr;
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // USDSCENE_LOADER_H
