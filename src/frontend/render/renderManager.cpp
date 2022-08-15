#include "renderManager.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
	delete scene;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
