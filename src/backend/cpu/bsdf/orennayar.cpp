#include "orennayar.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

OrenNayar::OrenNayar()
{
	_name = "OrenNayar";
	_roughness = 0.5f;
}

Vec3f OrenNayar::Evaluate(PixelSample& pixelSample,
		ShadingPoint& shadingPoint,
		BSDFSample& bsdfSample)
{
	// float roughnessSqrt(_roughness * _roughness);
	// We setup Oren-Nayar's specific terms.
	// float A = 1 - 0.5f * (roughnessSqrt / (roughnessSqrt + 0.57f));
	// float B = 0.45f * (roughnessSqrt / (roughnessSqrt + 0.09f));

	// TODO

	// TODO: "NdotL" should not be the same one as from "Sample()", but the actual dot product of the normal and the light direction?
	return Vec3f(0.0f);
}

Vec3f OrenNayar::Sample(PixelSample& pixelSample,
		ShadingPoint& shadingPoint,
		BSDFSample& bsdfSample)
{
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
	// return normalize(USDToSIMD(SIMDToUSD(wi) * shadingPoint.basis));
	return wi;
}

float OrenNayar::Pdf(PixelSample& pixelSample,
		ShadingPoint& shadingPoint,
		BSDFSample& bsdfSample)
{
	return bsdfSample.NdotL > 0.0f ? bsdfSample.NdotL / M_PI : 1.0f;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
