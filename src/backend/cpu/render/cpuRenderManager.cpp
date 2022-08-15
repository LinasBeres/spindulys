#include "cpuRenderManager.h"

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPURenderManager::CPURenderManager()
{
	scene = new CPUScene();
	mainCamera = std::unique_ptr<Camera>(new CPUCamera());
}

void CPURenderManager::Trace(int iterations)
{
	tbb::parallel_for(tbb::blocked_range<int>(0, renderGlobals.height), [&](tbb::blocked_range<int> height_range)
		{
			for (int pixelY = height_range.begin(); pixelY < height_range.end(); ++pixelY)
			{
				Sampler sampler;

				for (int pixelX = 0; pixelX < renderGlobals.width; ++pixelX)
				{
					// We setup all the necessary data describing the current sample.
					PixelSample pixelSample(sampler, pixelX, pixelY, pixelX + pixelY * renderGlobals.width, renderGlobals.samples, 0);

					// The final pixel color of the sample we are computed that will be added and averaged to the buffer.
					Col3f pixelColor(zero);

					for (int sample = 0; sample < renderGlobals.samples; ++sample)
					{
						Vec3f origin(zero);
						Vec3f direction(zero);
						mainCamera->GetCameraRay(pixelSample, origin, direction);
						Ray primaryRay(origin, direction);

						pixelColor += buffer.GetPixel(pixelSample.pixelIdx) * static_cast<float>(iterations - 1);

						if (renderGlobals.integratorID == IntegratorIds::UDPT)
							pixelColor += UDPTIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), renderGlobals);
						else if (renderGlobals.integratorID == IntegratorIds::Diffuse)
							pixelColor += DiffuseIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), renderGlobals);
						else if (renderGlobals.integratorID == IntegratorIds::Occlusion)
							pixelColor += OcclusionIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), renderGlobals);
						else if (renderGlobals.integratorID == IntegratorIds::Position)
							pixelColor += PositionIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), renderGlobals);
						else if (renderGlobals.integratorID == IntegratorIds::Normal)
							pixelColor += NormalIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), renderGlobals);
						else if (renderGlobals.integratorID == IntegratorIds::Debug)
							pixelColor += DebugIntegrator().GetPixelColor(primaryRay, pixelSample, dynamic_cast<CPUScene*>(scene), renderGlobals);

						pixelColor *= (1.f / renderGlobals.samples);

						++pixelSample.sampleIdx;
					}

					buffer.SetPixel(pixelSample.pixelIdx, pixelColor / static_cast<float>(iterations));
					// std::cerr << "pixel set to: " << buffer.GetPixel(pixelSample.pixelIdx) << "
				}
			}
		});
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
