#include "renderManager.h"

#include <filesystem>

#ifdef USING_USD
#include "../scene/usdTranslators/usdSceneLoader.h"
#endif


FRONTEND_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
	for (const auto& bufferID : renderGlobals.currentBufferIds)
		buffers[bufferID] = new Buffer3f(renderGlobals.width, renderGlobals.height);
}

RenderManager::~RenderManager()
{
	delete scene;

	for (const auto& bufferID : renderGlobals.currentBufferIds)
		delete buffers[bufferID];
}

bool RenderManager::ImportScene(const std::string& filepath)
{
	if (filepath.empty())
	{
		std::cerr << "Filepath is empty. Skipping.\n";
		return false;
	}

	const std::string ext = std::filesystem::path(filepath).extension();

#ifdef USING_USD
	if (ext == ".usd" || ext == ".usda" || ext == ".usdc" || ext == ".usdz")
	{
		UsdSceneLoader loader(scene);
		return loader.LoadScene(filepath);
	}
#endif

	std::cerr << "Unsupported filetype: " << ext << "\n";
	return false;
}

void RenderManager::LoadScene(const std::string& filepath)
{
	scene->ResetScene();

	ImportScene(filepath);

	if (scene->GetSceneCameras().empty())
		scene->CreateDefaultCamera();

	scene->CommitScene();
}

void RenderManager::Render()
{
	while (!stopRendererFunction())
	{
		if (updateRendererFunction)
			updateRendererFunction(this);

		if (update)
			ResetRender();

		if (iterations < renderGlobals.maxIterations)
		{
			iterations++;
			Trace(iterations);
		}

		if (drawBufferFunction)
			drawBufferFunction(renderGlobals.width, renderGlobals.height, *(buffers[renderGlobals.bufferID]));
	}
}

void RenderManager::ResetRender()
{
	iterations = 0;

	for (const auto& bufferID : renderGlobals.currentBufferIds)
		buffers[bufferID]->Clean(renderGlobals.width, renderGlobals.height);

	update = false;
}

bool RenderManager::AddBuffer(BufferIds bufferID)
{
	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (renderGlobals.currentBufferIds.find(bufferID) != renderGlobals.currentBufferIds.end())
		return false;

	renderGlobals.currentBufferIds.emplace(bufferID);
	buffers[bufferID] = new Buffer3f(renderGlobals.width, renderGlobals.height);

	return true;
}

bool RenderManager::RemoveBuffer(BufferIds bufferID)
{
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
