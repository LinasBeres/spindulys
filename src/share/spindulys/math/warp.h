#ifndef SPINDULYS_WARP_H
#define SPINDULYS_WARP_H

#include "../../spindulys.h"
#include "../platform.h"

#include "math.h"
#include "vec2.h"
#include "vec3.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

// =======================================================================
// Warping techniques that operate in the plane
// =======================================================================

__forceinline float circ(float x) { return safe_sqrt(madd(x, x, 1.f)); }

__forceinline Vec2f square_to_cosine_uniform_disk(const Vec2f& sample)
{
	float r = sqrt(sample.y);
	float _sin, _cos;
	sincosf(TwoPi<float> * sample.x, &_sin, &_cos);

	return { _cos * r, _sin * r};
}

__forceinline Vec2f square_to_uniform_disk_concentric(const Vec2f& sample)
{
	float x = msub(2.f, sample.x, 1.f);
	float y = msub(2.f, sample.y, 1.f);

	/* Modified concentric map code with less branching (by Dave Cline), see
		http://psgraphics.blogspot.ch/2011/01/improved-code-for-concentric-map.html
	*/
	float phi;
	float r;
	if (x == 0 && y == 0)
	{
		r = phi = 0;
	}
	else if (x * x > y * y)
	{
		r = x;
		phi = (Pi<float> / 4.f) * (y / x);
	}
	else
	{
		r = y;
		phi = (Pi<float> / 2.f) - (x / y) * (Pi<float> / 4.f);
	}

	float _sin, _cos;
	sincosf(phi, &_sin, &_cos);
	return { r * _cos, r * _sin };
}

// =======================================================================
// Warping techniques related to spheres and subsets
// =======================================================================

// Uniformly sample a vector on the unit sphere with respect to solid angles
__forceinline Vec3f square_to_uniform_sphere(const Vec2f& sample)
{
	float z = madd(2.f, sample.y, 1.f);
	float r = circ(z);

	float _sin, _cos;
	sincosf(2.f * Pi<float> * sample.x, &_sin, &_cos);
	return { r * _cos, r * _sin, z };
}

/// Inverse of the mapping square_to_uniform_sphere
__forceinline Vec2f uniform_sphere_to_square(const Vec3f& p) {
    float phi = atan2(p.y, p.x) * InvTwoPi<float>;
    return {
        select(phi < 0.f, phi + 1.f, phi),
        (1.f - p.z) * 0.5f
    };
}

// Density of square_to_uniform_sphere() with respect to solid angles
__forceinline float square_to_uniform_sphere_pdf()
{
	return InvFourPi<float>;
}

// =======================================================================

__forceinline Vec3f square_to_cosine_hemisphere(const Vec2f& sample)
{
	Vec2f p = square_to_uniform_disk_concentric(sample);

	float z = safe_sqrt(1.f - dot(p, p));

	return { p.x, p.y, z };
}

__forceinline float square_to_cosine_hemisphere_pdf(const Vec3f &v)
{
	return InvPi<float> * v.z;
}

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_WARP_H
