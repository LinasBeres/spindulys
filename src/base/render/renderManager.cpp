#include "renderManager.h"

#include <filesystem>

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include "../scene/objTranslators/objSceneLoader.h"

#ifdef USING_USD
#include "../scene/usdTranslators/usdSceneLoader.h"
#endif

BASE_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
	BASE_TRACE();
	for (const auto& bufferID : m_renderGlobals.GetCurrentBufferIds())
		m_buffers[bufferID] = new Buffer3f(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight());
}

RenderManager::~RenderManager()
{
	BASE_TRACE();
	delete m_scene;

	for (const auto& bufferID : m_renderGlobals.GetCurrentBufferIds())
		delete m_buffers[bufferID];
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
		ObjSceneLoader loader(m_scene);
		return loader.LoadScene(filepath);
	}
#ifdef USING_USD
	else if (ext == ".usd" || ext == ".usda" || ext == ".usdc" || ext == ".usdz")
	{
		UsdSceneLoader loader(m_scene);
		return loader.LoadScene(filepath);
	}
#endif

	spdlog::warn("Unsupported filetype: {}", ext);
	return false;
}

void RenderManager::LoadScene(const std::string& filepath)
{
	BASE_TRACE();
	m_scene->ResetScene();

	ImportScene(filepath);

	if (m_scene->GetSceneCameras().empty())
		m_scene->CreateDefaultCamera();

	m_scene->CommitScene();
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

	tbb::task_arena arena;

	while (!m_stopRendererFunction())
	{
		if (m_updateRendererFunction)
			m_updateRendererFunction();

		if (m_update)
			ResetRender();

		if (m_renderGlobals.GetScaleResolution() && m_frameSize < 1.f)
		{
			m_iterations = 0;
			m_frameSize += m_renderGlobals.GetGrowSize();
			m_currentResolution = Vec2i(m_frameSize * m_renderGlobals.GetWidth(), m_frameSize * m_renderGlobals.GetHeight());

			for (const auto& bufferID : m_renderGlobals.GetCurrentBufferIds())
				m_buffers[bufferID]->Clean(m_currentResolution.x, m_currentResolution.y);
			GetCamera().SetResolution(Vec2f(m_currentResolution.x, m_currentResolution.y));
		}

		if (m_iterations < m_renderGlobals.GetMaxIterations())
		{
			arena.execute( [&] {
			tbb::parallel_for(tbb::blocked_range<int>(0, m_currentResolution.y), [&](tbb::blocked_range<int> heightRange)
			{
				Trace(m_iterations, heightRange.begin(), heightRange.end());
				m_sampler->Advance();
			});
			const std::lock_guard<std::mutex> lock(GetLock());
			if (m_updateBufferFunction)
				m_updateBufferFunction(*(m_buffers[m_renderGlobals.GetBufferID()]));
			} );
			++m_iterations;
		}

		if (m_drawBufferFunction)
			m_drawBufferFunction(m_currentResolution.x, m_currentResolution.y, *(m_buffers[m_renderGlobals.GetBufferID()]));
	}
}

void RenderManager::ResetRender()
{
	BASE_TRACE();
	m_iterations = 1;
	m_frameSize = 0.f;

	m_currentResolution = Vec2i(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight());

	for (const auto& bufferID : m_renderGlobals.GetCurrentBufferIds())
		m_buffers[bufferID]->Clean(m_currentResolution.x, m_currentResolution.y);
	GetCamera().SetResolution(Vec2f(m_currentResolution.x, m_currentResolution.y));

	m_update = false;
}

bool RenderManager::AddBuffer(BufferIds bufferID)
{
	BASE_TRACE();
	if (!m_renderGlobals.AddBuffer(bufferID))
		return false;

	m_buffers[bufferID] = new Buffer3f(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight());

	return true;
}

bool RenderManager::RemoveBuffer(BufferIds bufferID)
{
	BASE_TRACE();
	if (!m_renderGlobals.RemoveBuffer(bufferID))
		return false;

	delete m_buffers[bufferID];

	return true;
}

bool RenderManager::SetSampler(SamplerIds samplerId)
{
	if (m_renderGlobals.SetSampler(samplerId))
	{
		switch (samplerId)
		{
			case (SamplerIds::kIndependent):
				m_sampler = std::make_unique<IndependentSampler>();
				break;
			case (SamplerIds::kUniform):
				m_sampler = std::make_unique<UniformSampler>();
				break;
			case (SamplerIds::kStratified):
				m_sampler = std::make_unique<StratifiedSampler>(/* spp: */ GetMaxIterations());
				break;
		}

		return true;
	}
	return false;
}

BASE_NAMESPACE_CLOSE_SCOPE
