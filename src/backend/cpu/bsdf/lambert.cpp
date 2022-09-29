#include "lambert.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

Lambert::Lambert()
{
	_name = "lambert";
}

Vec3f Lambert::Evaluate(PixelSample& pixelSample,
		ShadingPoint& shadingPoint,
		BSDFSample& bsdfSample)
{
	BACKEND_TRACE();
	// TODO: "NdotL" should not be the same one as from "Sample()", but the actual dot product of the normal and the light direction?
	const Vec3f geometryColor = Vec3f(shadingPoint.geometry.GetDisplayColor().r, shadingPoint.geometry.GetDisplayColor().g, shadingPoint.geometry.GetDisplayColor().b);
	return (geometryColor / static_cast<float>(M_PI)) * bsdfSample.NdotL;
}

Vec3f Lambert::Sample(PixelSample& pixelSample,
		ShadingPoint& shadingPoint,
		BSDFSample& bsdfSample)
{
	BACKEND_TRACE();
	float rand0 = pixelSample.sampler.Uniform1D();
	float rand1 = pixelSample.sampler.Uniform1D();
	Vec3f randomDirection(pixelSample.sampler.HemisphereCosineWeighted(rand0, rand1));

	float r = sqrt(rand0);
	float theta = 2.0f * M_PI * rand1;
	float phi = sqrt(1.0f - rand1);
	float x = r * cos(theta);
	float y = r * sin(theta);

	// We are orienting the random direction along the normal of the shading point,
	// somehow like when using a orthonormal basis.
	Vec3f u(normalize(cross(shadingPoint.Nw, randomDirection)));
	Vec3f v(cross(shadingPoint.Nw, u));

	Vec3f wi(shadingPoint.Nw);
	wi *= phi;
	wi += (u * x);
	wi += (v * y);

	bsdfSample.NdotL = dot(shadingPoint.Nw, wi);
	bsdfSample.pdf = Pdf(pixelSample, shadingPoint, bsdfSample);

	// TODO: Using the basis directly does not yield the same result. This should get looked into.
	return wi;
}

float Lambert::Pdf(PixelSample& pixelSample,
		ShadingPoint& shadingPoint,
		BSDFSample& bsdfSample)
{
	return bsdfSample.NdotL > 0.0f ? bsdfSample.NdotL / M_PI : 1.0f;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
