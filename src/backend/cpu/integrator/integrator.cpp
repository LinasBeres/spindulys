#include "integrator.h"

#include <spindulys/math/linearspace3.h>

#include <spindulys/mathHelper.h>


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

Integrator::Integrator()
{
}

ShadingPoint Integrator::SetupShadingPoint(const CPUScene* scene, const Ray& ray)
{
	ShadingPoint shadingPoint(scene->GetGeometery(ray.instID));

	shadingPoint.V = -Vec3f(ray.direction.x, ray.direction.y, ray.direction.z);
	shadingPoint.P = ray.origin + ray.tfar * ray.direction;

	// TODO: Use the normals primvar if available (for smooth normals).
	shadingPoint.N = normalize(ray.Ng);

	// Object to world space normal conversion.
	Vec3f normalWorld(xfmNormal(shadingPoint.geometry.GetTransform(), shadingPoint.N));
	// We make the normal faceforwarding.
	shadingPoint.Nw = normalize(dot(shadingPoint.V, normalWorld) < 0.0f ? -normalWorld : normalWorld);
	shadingPoint.UV = Vec2f(ray.u, ray.v);

	// We compute the tangent and bitangent from the normal.
	// TODO: Use the tangents primvar if available (for smooth tangents).
	Vec3f tangent(0.0f);
	Vec3f bitangent(0.0f);

	if(abs(shadingPoint.Nw.x) > abs(shadingPoint.Nw.y))
		tangent = Vec3f(-shadingPoint.Nw.z, 0.0f, shadingPoint.Nw.x) *
			(1.0f / sqrt(shadingPoint.Nw.x * shadingPoint.Nw.x +
													 shadingPoint.Nw.y * shadingPoint.Nw.y +
													 shadingPoint.Nw.z * shadingPoint.Nw.z));
	else
		tangent = Vec3f(0.0f, shadingPoint.Nw.z, -shadingPoint.Nw.y) *
			(1.0f / sqrt(shadingPoint.Nw.x * shadingPoint.Nw.x +
													 shadingPoint.Nw.y * shadingPoint.Nw.y +
													 shadingPoint.Nw.z * shadingPoint.Nw.z));

	bitangent = normalize(cross(shadingPoint.Nw, tangent));

	// We setup an orthogonal basis out of the normal, tangent and bitangent.
	shadingPoint.basis = LinearSpace3f(tangent.x, tangent.y, tangent.z,
			shadingPoint.Nw.x, shadingPoint.Nw.y, shadingPoint.Nw.z,
			bitangent.x, bitangent.y, bitangent.z);

	// We setup the last miscellaneous data of the shading point.
	shadingPoint.geomID = ray.geomID;
	shadingPoint.primID = ray.primID;
	shadingPoint.instID = ray.instID;
	shadingPoint.error = errorBias * std::max(
			std::max(std::abs(shadingPoint.P.x), abs(shadingPoint.P.y)),
			std::max(std::abs(shadingPoint.P.z), ray.tfar));

	return shadingPoint;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
