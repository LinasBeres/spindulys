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
	for (const auto& bufferID : renderGlobals.GetCurrentBufferIds())
		buffers[bufferID] = new Buffer3f(renderGlobals.GetWidth(), renderGlobals.GetHeight());

	sampler = std::make_unique<IndependentSampler>(32, 230);
}

RenderManager::~RenderManager()
{
	BASE_TRACE();
	delete scene;

	for (const auto& bufferID : renderGlobals.GetCurrentBufferIds())
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

	sampler->Seed(12312451u);

	while (!stopRendererFunction())
	{
		if (updateRendererFunction)
			updateRendererFunction(this);

		if (update)
			ResetRender();

		if (renderGlobals.GetScaleResolution() && frameSize < 1.f)
		{
			iterations = 0;
			frameSize += renderGlobals.GetGrowSize();
			currentResolution = Vec2i(frameSize * renderGlobals.GetWidth(), frameSize * renderGlobals.GetHeight());

			for (const auto& bufferID : renderGlobals.GetCurrentBufferIds())
				buffers[bufferID]->Clean(currentResolution.x, currentResolution.y);
			GetCamera().SetResolution(Vec2f(currentResolution.x, currentResolution.y));
		}

		if (iterations < renderGlobals.GetMaxIterations())
			Trace(++iterations);

		if (drawBufferFunction)
			drawBufferFunction(currentResolution.x, currentResolution.y, *(buffers[renderGlobals.GetBufferID()]));
	}
}

void RenderManager::ResetRender()
{
	BASE_TRACE();
	iterations = 0;
	frameSize = 0.f;

	currentResolution = Vec2i(renderGlobals.GetWidth(), renderGlobals.GetHeight());

	for (const auto& bufferID : renderGlobals.GetCurrentBufferIds())
		buffers[bufferID]->Clean(currentResolution.x, currentResolution.y);
	GetCamera().SetResolution(Vec2f(currentResolution.x, currentResolution.y));

	update = false;
}

bool RenderManager::AddBuffer(BufferIds bufferID)
{
	BASE_TRACE();
	if (!renderGlobals.AddBuffer(bufferID))
		return false;

	buffers[bufferID] = new Buffer3f(renderGlobals.GetWidth(), renderGlobals.GetHeight());

	return true;
}

bool RenderManager::RemoveBuffer(BufferIds bufferID)
{
	BASE_TRACE();
	if (!renderGlobals.RemoveBuffer(bufferID))
		return false;

	delete buffers[bufferID];

	return true;
}

BASE_NAMESPACE_CLOSE_SCOPE
