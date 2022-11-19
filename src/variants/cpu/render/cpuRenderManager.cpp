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
	tbb::parallel_for(tbb::blocked_range<int>(0, currentResolution.y), [&](tbb::blocked_range<int> height_range)
		{
			for (int pixelY = height_range.begin(); pixelY < height_range.end(); ++pixelY)
			{
				Sampler sampler;

				for (int pixelX = 0; pixelX < currentResolution.x; ++pixelX)
				{
					// We setup all the necessary data describing the current sample.
					PixelSample pixelSample(sampler, pixelX, pixelY, pixelX + pixelY * currentResolution.x, renderGlobals.GetMaxSamples(), 0);

					// The final pixel color of the sample we are computed that will be added and averaged to the buffer.
					Col3f pixelColor(zero);

					Vec3f origin(zero);
					Vec3f direction(zero);
					scene->GetSceneCamera().GetCameraRay(Vec2f(pixelX, pixelY), origin, direction);
					Ray primaryRay(origin, direction);

					buffers[BufferIds::kBeauty]->MultiplyPixel(pixelSample.pixelIdx, static_cast<float>(iterations - 1));

					const auto [color, _] = integrator->Sample(dynamic_cast<CPUScene*>(scene), pixelSample, primaryRay, nullptr);

					buffers[BufferIds::kBeauty]->AddPixel(pixelSample.pixelIdx, color);

					++pixelSample.sampleIdx;

					buffers[BufferIds::kBeauty]->MultiplyPixel(pixelSample.pixelIdx, 1.f / static_cast<float>(iterations));
				}
			}
			});
}

bool CPURenderManager::SetIntegrator(IntegratorIds integratorID)
{
	if (!RenderManager::SetIntegrator(integratorID))
		return false;

	InitialiseIntegrator(integratorID);
	return true;
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
