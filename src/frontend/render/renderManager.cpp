#include "renderManager.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
	for (const auto& bufferID : renderGlobals.currentBufferIds)
		buffers[bufferID] = std::unique_ptr<Buffer3f>(new Buffer3f(renderGlobals.width, renderGlobals.height));
}

RenderManager::~RenderManager()
{
	delete scene;
}

void RenderManager::Render()
{
	while (!stopRendererFunction())
	{
		if (updateRendererFunction)
			updateRendererFunction(this);

		if (update)
			ResetRender();

		if (iterations++ < renderGlobals.maxIterations)
			Trace(iterations);

		if (drawBufferFunction)
			drawBufferFunction(renderGlobals.width, renderGlobals.height, buffer);
	}
}

void RenderManager::ResetRender()
{
	iterations = 0;
	buffer.Clean(renderGlobals.width, renderGlobals.height);

	update = false;
}

bool RenderManager::AddBuffer(BufferIds bufferID)
{
	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (renderGlobals.currentBufferIds.find(bufferID) != renderGlobals.currentBufferIds.end())
		return false;

	renderGlobals.currentBufferIds.emplace(bufferID);
	buffers[bufferID] = std::unique_ptr<Buffer3f>(new Buffer3f(renderGlobals.width, renderGlobals.height));

	return true;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
