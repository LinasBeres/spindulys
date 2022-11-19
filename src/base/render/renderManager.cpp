#include "renderManager.h"

#include <filesystem>

#include "../scene/objTranslators/objSceneLoader.h"

#ifdef USING_USD
#include "../scene/usdTranslators/usdSceneLoader.h"
#endif

BASE_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
	BASE_TRACE();
	for (const auto& bufferID : renderGlobals.currentBufferIds)
		buffers[bufferID] = new Buffer3f(renderGlobals.width, renderGlobals.height);
}

RenderManager::~RenderManager()
{
	BASE_TRACE();
	delete scene;

	for (const auto& bufferID : renderGlobals.currentBufferIds)
		delete buffers[bufferID];
}

bool RenderManager::ImportScene(const std::string& filepath)
{
	BASE_TRACE();
	if (filepath.empty())
	{
		spdlog::warn("Filepath is empty. Skipping");
		return false;
	}

	const std::string ext = std::filesystem::path(filepath).extension();

	if (ext == ".obj")
	{
		ObjSceneLoader loader(scene);
		return loader.LoadScene(filepath);
	}
#ifdef USING_USD
	else if (ext == ".usd" || ext == ".usda" || ext == ".usdc" || ext == ".usdz")
	{
		UsdSceneLoader loader(scene);
		return loader.LoadScene(filepath);
	}
#endif

	spdlog::warn("Unsupported filetype: {}", ext);
	return false;
}

void RenderManager::LoadScene(const std::string& filepath)
{
	BASE_TRACE();
	scene->ResetScene();

	ImportScene(filepath);

	if (scene->GetSceneCameras().empty())
		scene->CreateDefaultCamera();

	scene->CommitScene();
}

const std::string_view RenderManager::ValidSceneFormats()
{
	BASE_TRACE();
#ifdef USING_USD
	return std::string_view("obj, usd, usda, usdc, usdz");
#else
	return std::string_view("obj");
#endif
}

void RenderManager::Render()
{
	BASE_TRACE();
	while (!stopRendererFunction())
	{
		if (updateRendererFunction)
			updateRendererFunction(this);

		if (update)
			ResetRender();

		if (renderGlobals.scaleResolution && frameSize < 1.f)
		{
			iterations = 0;
			frameSize += renderGlobals.growSize;
			currentResolution = Vec2i(frameSize * renderGlobals.width, frameSize * renderGlobals.height);

			for (const auto& bufferID : renderGlobals.currentBufferIds)
				buffers[bufferID]->Clean(currentResolution.x, currentResolution.y);
			GetCamera().SetResolution(Vec2f(currentResolution.x, currentResolution.y));
		}

		if (iterations < renderGlobals.maxIterations)
			Trace(++iterations);

		if (drawBufferFunction)
			drawBufferFunction(currentResolution.x, currentResolution.y, *(buffers[renderGlobals.bufferID]));
	}
}

void RenderManager::ResetRender()
{
	BASE_TRACE();
	iterations = 0;
	frameSize = 0.f;

	currentResolution = Vec2i(renderGlobals.width, renderGlobals.height);

	for (const auto& bufferID : renderGlobals.currentBufferIds)
		buffers[bufferID]->Clean(currentResolution.x, currentResolution.y);
	GetCamera().SetResolution(Vec2f(currentResolution.x, currentResolution.y));

	update = false;
}

bool RenderManager::AddBuffer(BufferIds bufferID)
{
	BASE_TRACE();
	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (renderGlobals.currentBufferIds.find(bufferID) != renderGlobals.currentBufferIds.end())
		return false;

	renderGlobals.currentBufferIds.emplace(bufferID);
	buffers[bufferID] = new Buffer3f(renderGlobals.width, renderGlobals.height);

	return true;
}

bool RenderManager::RemoveBuffer(BufferIds bufferID)
{
	BASE_TRACE();
	// Beauty cannot be removed
	if (bufferID == BufferIds::kBeauty)
		return false;

	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (renderGlobals.currentBufferIds.find(bufferID) != renderGlobals.currentBufferIds.end())
		return false;

	renderGlobals.currentBufferIds.erase(bufferID);
	delete buffers[bufferID];

	return true;
}

BASE_NAMESPACE_CLOSE_SCOPE
