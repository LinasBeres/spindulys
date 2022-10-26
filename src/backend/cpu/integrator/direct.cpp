#include "direct.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

Direct::Direct() { }


void Direct::GetPixelColor(Ray& ray,
		PixelSample& pixelSample,
		CPUScene* scene,
		RenderManager::Buffers buffers,
		const RenderManager::RenderGlobals& renderGlobals)
{
	BACKEND_TRACE();

	RTCIntersectContext intersectContext;
	rtcInitIntersectContext(&intersectContext);
	rtcIntersect1(scene->GetScene(), &intersectContext, RTCRayHit_(ray));

	// TODO: Hardcoded sky color value for now.
	if (ray.instID == RTC_INVALID_GEOMETRY_ID)
	{
		return;
	}

	// We setup all the necessary data describing the shading point.
	// ShadingPoint shadingPoint(SetupShadingPoint(scene, ray));


}

std::pair<Col3f, float> Direct::Sample(const CPUScene* scene, PixelSample* pixelSample, const Ray& ray, Col3f* /* aovs */) const
{
	Col3f result(zero);

	// SurfaceInteraction si = scene->RayIntersect(ray);

	// bool validRay = si.IsValid();

	// ----------------------- Visible emitters -----------------------
	// TODO:

	// ----------------------- Emitter sampling -----------------------
	// TODO:

	// ------------------------ BSDF sampling -------------------------
	// TODO:

	return { result, 0.f };
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
