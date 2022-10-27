#include "direct.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

Direct::Direct() { }


std::pair<Col3f, float> Direct::Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */)
{
	Col3f result(zero);

	SurfaceInteraction si = scene->RayIntersect(ray);

	// ----------------------- Visible emitters -----------------------
	// TODO:

	if (!si.IsValid())
		return { Col3f(0.7, 0.8, 0.9), 0.f };

	// ----------------------- Emitter sampling -----------------------
	// TODO:

	// ------------------------ BSDF sampling -------------------------
	// TODO:
	BSDFSample bsdfSample;
	bsdfSample.wi = diffuseMat.Sample2(pixelSample, si, bsdfSample);
	bsdfSample.reflectance = diffuseMat.Evaluate2(pixelSample, si, bsdfSample);

	float directionSign(sign(dot(bsdfSample.wi, si.shadingFrame.vz)));
	Vec3f origin = si.p + (directionSign * 32.0f * 1.19209e-07f * si.shadingFrame.vz);
	Vec3f direction = bsdfSample.wi;

	// Initializing the new ray.
	Ray rh = Ray(origin, direction, 32.0f * 1.19209e-07f);
	if (rh.time != 0) { ; }
	const Vec3f color = bsdfSample.reflectance / bsdfSample.pdf;

	result = result * Col3f(color.x, color.y, color.z);

	return { result, 0.f };
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
