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

		if (iterations++ < renderGlobals.maxIterations)
			Trace(iterations);

		if (drawBufferFunction)
			drawBufferFunction(renderGlobals.width, renderGlobals.height, buffer);
	}
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
