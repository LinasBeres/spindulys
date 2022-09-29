#include "udpt.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

UDPTIntegrator::UDPTIntegrator() { }


void UDPTIntegrator::GetPixelColor(Ray& ray,
		PixelSample& pixelSample,
		CPUScene* scene,
		RenderManager::Buffers buffers,
		const RenderManager::RenderGlobals& renderGlobals)
{
	BACKEND_TRACE();
	Col3f colorAccumulation(0.0f);
	Col3f colorThroughput(1.0f);

	for (int bounce = 0; bounce < renderGlobals.depth; ++bounce)
	{
		RTCIntersectContext intersectContext;
		rtcInitIntersectContext(&intersectContext);
		rtcIntersect1(scene->GetScene(), &intersectContext, RTCRayHit_(ray));

		// TODO: Hardcoded sky color value for now.
		if (ray.instID == RTC_INVALID_GEOMETRY_ID)
		{
			for (const auto& bufferID : renderGlobals.currentBufferIds)
				buffers[bufferID]->AddPixel(pixelSample.pixelIdx, colorThroughput * Col3f(0.7, 0.8, 0.9));
			return;
		}

		// We setup all the necessary data describing the shading point.
		ShadingPoint shadingPoint(SetupShadingPoint(scene, ray));

		// Normal, position and debug buffers
		if (bounce == 0)
		{
			if(renderGlobals.currentBufferIds.find(RenderManager::BufferIds::Normal) != renderGlobals.currentBufferIds.end())
			{
				buffers[RenderManager::BufferIds::Normal]->AddPixel(pixelSample.pixelIdx, Col3f(shadingPoint.N.x, shadingPoint.N.y, shadingPoint.N.z));
			}
			if(renderGlobals.currentBufferIds.find(RenderManager::BufferIds::Position) != renderGlobals.currentBufferIds.end())
			{
				const Vec3f normalizedPoint = normalize(shadingPoint.P);
				buffers[RenderManager::BufferIds::Position]->AddPixel(pixelSample.pixelIdx, Col3f(normalizedPoint.x, normalizedPoint.y, normalizedPoint.z));
			}
			if(renderGlobals.currentBufferIds.find(RenderManager::BufferIds::Debug) != renderGlobals.currentBufferIds.end())
			{
				const Col3f debugPoint((static_cast<float>((shadingPoint.instID & 0x000000ff) >>  0)) / 255.0f,
						(static_cast<float>((shadingPoint.instID & 0x0000ff00) >>  8)) / 255.0f,
						(static_cast<float>((shadingPoint.instID & 0x00ff0000) >> 16)) / 255.0f);
				buffers[RenderManager::BufferIds::Debug]->AddPixel(pixelSample.pixelIdx, debugPoint);
			}
			if(renderGlobals.currentBufferIds.find(RenderManager::BufferIds::Diffuse) != renderGlobals.currentBufferIds.end())
			{
				const float diffuse(std::fabs(dot(shadingPoint.Nw, Vec3f(ray.direction.x, ray.direction.y, ray.direction.z))));
				const Col3f diffusePoint(shadingPoint.geometry.GetDisplayColor() * diffuse * (1.0f / static_cast<float>(M_PI)));
				buffers[RenderManager::BufferIds::Diffuse]->AddPixel(pixelSample.pixelIdx, diffusePoint);
			}
		}

		// Sky/Environment Sampling
		// TODO

		// Light Sampling/Next Event Estimation
		// TODO

		// BSDF Sampling
		BSDFSample bsdfSample;
		// TODO: We should use a proper material instead of a BSDF/Lobe directly.
		bsdfSample.wi = diffuseMat.Sample(pixelSample, shadingPoint, bsdfSample);
		bsdfSample.reflectance = diffuseMat.Evaluate(pixelSample, shadingPoint, bsdfSample);

		// Using the world-space normal and the error bias of the shading point , as well as a sign,
		// we apply some form of jitter on the position of the shading point,
		// effectively offsetting the origin of the following ray.
		float directionSign(sign(dot(bsdfSample.wi, shadingPoint.Nw)));
		ray.origin = shadingPoint.P + (directionSign * shadingPoint.error * shadingPoint.Nw);
		ray.direction = bsdfSample.wi;

		// Initializing the new ray.
		ray = Ray(ray.origin, ray.direction, shadingPoint.error);
		const Vec3f color = bsdfSample.reflectance / bsdfSample.pdf;

		colorThroughput = colorThroughput * Col3f(color.x, color.y, color.z);
	}

	buffers[RenderManager::BufferIds::Beauty]->AddPixel(pixelSample.pixelIdx, colorAccumulation);
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
