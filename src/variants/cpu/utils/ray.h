#ifndef CPU_RAY_H
#define CPU_RAY_H

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <spindulys/math/math.h>
#include <spindulys/math/vec3.h>
#include <spindulys/sampler.h>

#include <utils/helperStructs.h>

#include "../spindulysCPU.h"

CPU_NAMESPACE_OPEN_SCOPE

// -----------------------------------------------------------------------------

struct __aligned(16) Ray
{
	Vec3f origin;
	float tnear;
	Vec3f direction;
	float time;

	float tfar;
	unsigned int mask;
	unsigned int id;
	unsigned int flags;

	Vec3f Ng;
	float u;
	float v;
	unsigned int primID;
	unsigned int geomID;
	unsigned int instID;

	__forceinline Ray(const Vec3f& origin,
			const Vec3f& direction,
			float tnear = 0.0f,
			float tfar = std::numeric_limits<float>::infinity(),
			float time = 0.0f,
			int mask = -1,
			unsigned int geomID = RTC_INVALID_GEOMETRY_ID,
			unsigned int primID = RTC_INVALID_GEOMETRY_ID,
			unsigned int instID = RTC_INVALID_GEOMETRY_ID)
		: origin(origin),
		tnear(tnear),
		direction(direction),
		time(time),
		tfar(tfar),
		mask(mask),
		primID(primID),
		geomID(geomID),
		instID(instID)
	{
	}

};

__forceinline RTCRay *RTCRay_(Ray& ray)
{
	return (RTCRay *) &ray;
}

__forceinline RTCRayHit *RTCRayHit_(Ray& ray)
{
	return (RTCRayHit *) &ray;
}

__forceinline RTCRayHit *RTCRayHit_(const Ray& ray)
{
	return (RTCRayHit *) &ray;
}

// -----------------------------------------------------------------------------

/**
 * \brief This list of flags is used to determine which members of \ref SurfaceInteraction
 * should be computed when calling \ref compute_surface_interaction().
 *
 * It also specifies whether the \ref SurfaceInteraction should be differentiable with
 * respect to the shapes parameters.
 */
enum class RayFlags : uint32_t {

	// =============================================================
	//             Surface interaction compute flags
	// =============================================================

	/// No flags set
	Empty = 0x0,

	/// Compute position and geometric normal
	Minimal = 0x1,

	/// Compute UV coordinates
	UV = 0x2,

	/// Compute position partials wrt. UV coordinates
	dPdUV = 0x4,

	/// Compute shading normal and shading frame
	ShadingFrame = 0x8,

	/// Compute the geometric normal partials wrt. the UV coordinates
	dNGdUV = 0x10,

	/// Compute the shading normal partials wrt. the UV coordinates
	dNSdUV = 0x20,

	/// Compute the boundary-test used in reparameterized integrators
	BoundaryTest = 0x40,

	// =============================================================
	//!              Differentiability compute flags
	// =============================================================

	/// Derivatives of the SurfaceInteraction fields follow shape's motion
	FollowShape = 0x80,

	/// Derivatives of the SurfaceInteraction fields ignore shape's motion
	DetachShape = 0x100,

	// =============================================================
	//!                 Compound compute flags
	// =============================================================

	/* \brief Default: compute all fields of the surface interaction data
		 structure except shading/geometric normal derivatives */
	All = UV | dPdUV | ShadingFrame,

	/// Compute all fields of the surface interaction ignoring shape's motion
	AllNonDifferentiable = All | DetachShape,
};

// -----------------------------------------------------------------------------

CPU_NAMESPACE_CLOSE_SCOPE

#endif // RAY_H
