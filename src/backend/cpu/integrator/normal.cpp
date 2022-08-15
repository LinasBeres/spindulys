#include "normal.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

NormalIntegrator::NormalIntegrator() { }

Col3f NormalIntegrator::GetPixelColor(Ray& ray,
		PixelSample& pixelSample,
		CPUScene* scene,
		const RenderManager::RenderGlobals& renderGlobals)
{
	RTCIntersectContext intersectContext;
	rtcInitIntersectContext(&intersectContext);

	rtcIntersect1(scene->GetScene(), &intersectContext, RTCRayHit_(ray));

	// TODO: Hardcoded sky color value for now.
	if (ray.instID == RTC_INVALID_GEOMETRY_ID)
	{
		return Col3f(0.7, 0.8, 0.9);
	}

	// We setup all the necessary data describing the shading point.
	ShadingPoint shadingPoint(SetupShadingPoint(scene, ray));

	return Col3f(shadingPoint.N.x, shadingPoint.N.y, shadingPoint.N.z);
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
