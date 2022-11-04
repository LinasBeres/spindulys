#include "cpuRenderManager.h"

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include "../integrator/direct.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPURenderManager::CPURenderManager()
{
	scene = new CPUScene();
}

void CPURenderManager::Trace(int iterations)
{
	BACKEND_TRACE();
	// tbb::parallel_for(tbb::blocked_range<int>(0, currentResolution.y), [&](tbb::blocked_range<int> height_range)
		// {
			for (int pixelY = 0; pixelY < currentResolution.y; ++pixelY)
			{
				Sampler sampler;

				for (int pixelX = 0; pixelX < currentResolution.x; ++pixelX)
				{
					// We setup all the necessary data describing the current sample.
					PixelSample pixelSample(sampler, pixelX, pixelY, pixelX + pixelY * currentResolution.x, renderGlobals.samples, 0);

					// The final pixel color of the sample we are computed that will be added and averaged to the buffer.
					Col3f pixelColor(zero);

					for (int sample = 0; sample < renderGlobals.samples; ++sample)
					{
						Vec3f origin(zero);
						Vec3f direction(zero);
						scene->GetSceneCamera().GetCameraRay(Vec2f(pixelX, pixelY), origin, direction);
						Ray primaryRay(origin, direction);

						for (const auto& bufferID : renderGlobals.currentBufferIds)
							buffers[bufferID]->MultiplyPixel(pixelSample.pixelIdx, static_cast<float>(iterations - 1));

						// buffers[BufferIds::Beauty]->MultiplyPixel(pixelSample.pixelIdx, static_cast<float>(iterations - 1));

						// if (renderGlobals.integratorID == IntegratorIds::UDPT)
							// UDPTIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), buffers, renderGlobals);
						if (renderGlobals.integratorID == IntegratorIds::UDPT)
						{
							const auto [color, mask] = Direct().Sample(dynamic_cast<CPUScene*>(scene), pixelSample, primaryRay, nullptr);
							if (mask) { ; }
							buffers[RenderManager::BufferIds::Beauty]->AddPixel(pixelSample.pixelIdx, color);
						}

						for (const auto& bufferID : renderGlobals.currentBufferIds)
							buffers[bufferID]->MultiplyPixel(pixelSample.pixelIdx, 1.f / renderGlobals.samples);

						++pixelSample.sampleIdx;
					}

					for (const auto& bufferID : renderGlobals.currentBufferIds)
						buffers[bufferID]->MultiplyPixel(pixelSample.pixelIdx, 1.f / static_cast<float>(iterations));
				}
			}
		// });
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
