#include "renderManager.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
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

FRONTEND_NAMESPACE_CLOSE_SCOPE
