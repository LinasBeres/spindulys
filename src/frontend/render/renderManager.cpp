#include "renderManager.h"

#include <filesystem>

#include "../scene/objTranslators/objSceneLoader.h"

#ifdef USING_USD
#include "../scene/usdTranslators/usdSceneLoader.h"
#endif

FRONTEND_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
	FRONTEND_TRACE();
	for (const auto& bufferID : renderGlobals.currentBufferIds)
		buffers[bufferID] = new Buffer3f(renderGlobals.width, renderGlobals.height);
}

RenderManager::~RenderManager()
{
	FRONTEND_TRACE();
	delete scene;

	for (const auto& bufferID : renderGlobals.currentBufferIds)
		delete buffers[bufferID];
}

bool RenderManager::ImportScene(const std::string& filepath)
{
	FRONTEND_TRACE();
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
	FRONTEND_TRACE();
	scene->ResetScene();

	ImportScene(filepath);

	if (scene->GetSceneCameras().empty())
		scene->CreateDefaultCamera();

	scene->CommitScene();
}

const std::string_view RenderManager::ValidSceneFormats()
{
	FRONTEND_TRACE();
#ifdef USING_USD
	return std::string_view("obj, usd, usda, usdc, usdz");
#else
	return std::string_view("obj");
#endif
}

void RenderManager::Render()
{
	FRONTEND_TRACE();
	while (!stopRendererFunction())
	{
		if (updateRendererFunction)
			updateRendererFunction(this);

		if (update)
			ResetRender();

		if (renderGlobals.scaleResolution && frameSize < 1.f)
		{
			iterations = 0;
			frameSize += GROW_SIZE;
			currentResolution = Vec2i(frameSize * renderGlobals.width, frameSize * renderGlobals.height);

			for (const auto& bufferID : renderGlobals.currentBufferIds)
				buffers[bufferID]->Clean(currentResolution.x, currentResolution.y);
			GetCamera().SetResolution(Vec2f(currentResolution.x, currentResolution.y));
		}

		if (iterations < renderGlobals.maxIterations)
		{
			iterations++;
			Trace(iterations);
		}

		if (drawBufferFunction)
			drawBufferFunction(currentResolution.x, currentResolution.y, *(buffers[renderGlobals.bufferID]));
	}
}

void RenderManager::ResetRender()
{
	FRONTEND_TRACE();
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
	FRONTEND_TRACE();
	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (renderGlobals.currentBufferIds.find(bufferID) != renderGlobals.currentBufferIds.end())
		return false;

	renderGlobals.currentBufferIds.emplace(bufferID);
	buffers[bufferID] = new Buffer3f(renderGlobals.width, renderGlobals.height);

	return true;
}

bool RenderManager::RemoveBuffer(BufferIds bufferID)
{
	FRONTEND_TRACE();
	// Beauty cannot be removed
	if (bufferID == BufferIds::Beauty)
		return false;

	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (renderGlobals.currentBufferIds.find(bufferID) != renderGlobals.currentBufferIds.end())
		return false;

	renderGlobals.currentBufferIds.erase(bufferID);
	delete buffers[bufferID];

	return true;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
