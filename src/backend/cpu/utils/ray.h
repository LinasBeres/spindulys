#ifndef CPU_RAY_H
#define CPU_RAY_H

#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>

#include <spindulys/math/math.h>
#include <spindulys/math/vec3.h>
#include <spindulys/sampler.h>

#include <camera/camera.h>

#include <utils/helperStructs.h>

#include "../spindulysBackendCPU.h"



FRONTEND_NAMESPACE_OPEN_SCOPE

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

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // RAY_H
