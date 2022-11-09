#ifndef USDSCENE_LOADER_H
#define USDSCENE_LOADER_H

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>

#include "../../spindulysBase.h"

#include "../scene.h"
#include "../sceneLoader.h"

BASE_NAMESPACE_OPEN_SCOPE

class UsdSceneLoader final : public SceneLoader
{
	public:
		UsdSceneLoader(Scene* scene) : SceneLoader(scene) { };
		~UsdSceneLoader() = default;

		virtual bool LoadScene(const std::string& filepath) override;

	private:
		bool LoadPrims(const pxr::UsdStagePtr& stage, const pxr::SdfPath& primPath);
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // USDSCENE_LOADER_H
