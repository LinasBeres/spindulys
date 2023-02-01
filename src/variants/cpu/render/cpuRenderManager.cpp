#include "cpuRenderManager.h"

#include "../integrator/direct.h"
#include "../integrator/forwardPath.h"

CPU_NAMESPACE_OPEN_SCOPE

CPURenderManager::CPURenderManager()
{
	scene = new CPUScene();

	InitialiseIntegrator(renderGlobals.GetIntegrator());
}

void CPURenderManager::Trace(int iterations, size_t heightBegin, size_t heightEnd)
{
	CPU_TRACE();
	uint32_t seed = ((heightBegin - heightEnd) * currentResolution.x) * heightBegin;
	seed += std::hash<std::thread::id>{}(std::this_thread::get_id());
	seed *= iterations;

	Sampler* workerSampler = nullptr;
	workerSampler = sampler->Fork();

	for (size_t pixelY = heightBegin; pixelY < heightEnd; ++pixelY)
	{
		for (int pixelX = 0; pixelX < currentResolution.x; ++pixelX)
		{
			workerSampler->Seed(seed++);

			// We setup all the necessary data describing the current sample.
			const uint32_t pixelIdx = pixelX + pixelY * currentResolution.x;

			// The final pixel color of the sample we are computed that will be added and averaged to the buffer.
			Col3f pixelColor(zero);

			// Construct camera ray
			const Vec2f cameraSample = Vec2f(pixelX, pixelY) + workerSampler->Next2d();
			Vec3f origin(zero);
			Vec3f direction(zero);
			scene->GetSceneCamera().GetCameraRay(cameraSample, origin, direction);
			Ray primaryRay(origin, direction);

			const auto [color, _] = integrator->Sample(dynamic_cast<CPUScene*>(scene), workerSampler, primaryRay, nullptr);

			buffers[BufferIds::kBeauty]->MultiplyPixel(pixelIdx, static_cast<float>(iterations - 1));
			buffers[BufferIds::kBeauty]->AddPixel(pixelIdx, color);
			buffers[BufferIds::kBeauty]->MultiplyPixel(pixelIdx, 1.f / static_cast<float>(iterations));
		}
	}

	delete workerSampler;
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
