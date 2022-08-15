#include "udpt.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

UDPTIntegrator::UDPTIntegrator() { }

Col3f UDPTIntegrator::GetPixelColor(Ray& ray,
		PixelSample& pixelSample,
		CPUScene* scene,
		const RenderManager::RenderGlobals& renderGlobals)
{
	Col3f colorAccumulation(0.0f);
	Col3f colorThroughput(1.0f);

	for (int bounce = 0; bounce < renderGlobals.depth; ++bounce)
	{
		RTCIntersectContext intersectContext;
		rtcInitIntersectContext(&intersectContext);

		rtcIntersect1(scene->GetScene(), &intersectContext, RTCRayHit_(ray));

		if (ray.instID == RTC_INVALID_GEOMETRY_ID)
		{
			// TODO: Hardcoded sky color value for now.
			return colorAccumulation += colorThroughput * Col3f(0.7, 0.8, 0.9);
		}

		// We setup all the necessary data describing the shading point.
		ShadingPoint shadingPoint(SetupShadingPoint(scene, ray));

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

	return colorAccumulation;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
