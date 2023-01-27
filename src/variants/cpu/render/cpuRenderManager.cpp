#include "cpuRenderManager.h"

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include "../integrator/direct.h"
#include "../integrator/forwardPath.h"

CPU_NAMESPACE_OPEN_SCOPE

CPURenderManager::CPURenderManager()
{
	scene = new CPUScene();

	InitialiseIntegrator(renderGlobals.GetIntegrator());
}

void CPURenderManager::Trace(int iterations)
{
	CPU_TRACE();
	tbb::parallel_for(tbb::blocked_range<int>(0, currentResolution.y), [&](tbb::blocked_range<int> heightRange)
		{
			uint32_t seed = iterations * (heightRange.begin() * heightRange.end());
			Sampler* workerSampler = sampler->Fork();

			for (int pixelY = heightRange.begin(); pixelY < heightRange.end(); ++pixelY)
			{
				for (int pixelX = 0; pixelX < currentResolution.x; ++pixelX)
				{
					// We setup all the necessary data describing the current sample.
					const uint32_t pixelIdx = pixelX + pixelY * currentResolution.x;
					workerSampler->Seed(seed + pixelIdx);

					// The final pixel color of the sample we are computed that will be added and averaged to the buffer.
					Col3f pixelColor(zero);

					Vec3f origin(zero);
					Vec3f direction(zero);
					scene->GetSceneCamera().GetCameraRay(Vec2f(pixelX, pixelY), origin, direction);
					Ray primaryRay(origin, direction);

					buffers[BufferIds::kBeauty]->MultiplyPixel(pixelIdx, static_cast<float>(iterations - 1));

					const auto [color, _] = integrator->Sample(dynamic_cast<CPUScene*>(scene), workerSampler, primaryRay, nullptr);

					buffers[BufferIds::kBeauty]->AddPixel(pixelIdx, color);

					buffers[BufferIds::kBeauty]->MultiplyPixel(pixelIdx, 1.f / static_cast<float>(iterations));

				}
			}

			delete workerSampler;
			});
}

bool CPURenderManager::SetIntegrator(IntegratorIds integratorID)
{
	if (!RenderManager::SetIntegrator(integratorID))
		return false;

	InitialiseIntegrator(integratorID);
	return true;
}

bool CPURenderManager::SetMaxLightSamples(uint32_t samples)
{
	if (RenderManager::SetMaxLightSamples(samples))
		if (Direct* directIntegrator = dynamic_cast<Direct*>(integrator.get()))
			return directIntegrator->SetLightSamples(samples);

	return false;
}

bool CPURenderManager::SetMaxBSDFSamples(uint32_t samples)
{
	if (RenderManager::SetMaxBSDFSamples(samples))
		if (Direct* directIntegrator = dynamic_cast<Direct*>(integrator.get()))
			return directIntegrator->SetBSDFSamples(samples);

	return false;
}

bool CPURenderManager::SetMaxDepth(uint32_t depth)
{
	if (RenderManager::SetMaxDepth(depth))
		if (ForwardPath* forwardIntegrator = dynamic_cast<ForwardPath*>(integrator.get()))
			return forwardIntegrator->SetMaxDepth(depth);

	return false;
}

bool CPURenderManager::SetRussianRouletteDepth(uint32_t depth)
{
	if (RenderManager::SetRussianRouletteDepth(depth))
		if (ForwardPath* forwardIntegrator = dynamic_cast<ForwardPath*>(integrator.get()))
			return forwardIntegrator->SetRussianRouletteDepth(depth);

	return false;
}

void CPURenderManager::InitialiseIntegrator(IntegratorIds integratorID)
{
	switch (renderGlobals.GetIntegrator())
	{
		case (IntegratorIds::kDirect):
			integrator = std::make_unique<Direct>(renderGlobals.GetMaxLightsSamples(), renderGlobals.GetMaxBSDFSamples(), renderGlobals.GetHideLights());
			break;
		case (IntegratorIds::kForwardPath):
			integrator = std::make_unique<ForwardPath>(renderGlobals.GetMaxDepth(), renderGlobals.GetRussianRouletteDepth(), renderGlobals.GetHideLights());
			break;
	}
}

CPU_NAMESPACE_CLOSE_SCOPE
