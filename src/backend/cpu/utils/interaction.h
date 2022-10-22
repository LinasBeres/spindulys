#ifndef CPU_INTERACTION_H
#define CPU_INTERACTION_H

#include <spindulys/math/constants.h>
#include <spindulys/math/vec3.h>
#include <spindulys/math/linearspace3.h>

#include "../spindulysBackendCPU.h"

#include "../geometry/cpuGeometry.h"
#include "ray.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

template <typename T> constexpr auto RayEpsilon = Epsilon<T> * 1500;
template <typename T> constexpr auto ShadowEpsilon = RayEpsilon<T> * 10;

// Generic surface interaction data structure
struct Interaction
{
	// Distance traveled along the ray
	float t = Infinity<float>;

	// Time value associated with the interaction
	float time = 0.f;

	// Position of the interaction in world coordinates
	Vec3f p = Vec3f(zero);

	// Geometric normal (only valid for \c SurfaceInteraction)
	Vec3f n = Vec3f(zero);

	// Constructor
	Interaction() {}
	Interaction(float t, float time, const Vec3f& p, const Vec3f& n = Vec3f(zero))
		: t(t), time(time), p(p), n(n) { }

	// Is the current interaction valid?
	int IsValid() const
	{
		return t != static_cast<float>(PosInfTy());
	}

	// Spawn a semi-infinite ray towards the given direction
	Ray SpawnRay(const Vec3f& d) const
	{
		return Ray(OffsetP(d), d, 0.f, Largest<float>, time);
	}

	// Spawn a finite ray towards the given position
	Ray SpawnRayTo(const Vec3f& t) const
	{
		Vec3f o = OffsetP(t - p);
		Vec3f d = t - o;
		float dist = norm(d);
		d /= dist;
		return Ray(o, d, 0.f, dist * (1.f - ShadowEpsilon<float>), time);
	}

	private:
		/**
		 * Compute an offset position, used when spawning a ray from this
		 * interaction. When the interaction is on the surface of a shape, the
		 * position is offset along the surface normal to prevent self intersection.
		 */
		Vec3f OffsetP(const Vec3f& d) const
		{
			float mag = (1.f + reduce_max(abs(p))) * RayEpsilon<float>;
			mag = dot(n, d) >= 0 ? mag : -mag;
			return madd(mag, n, p);
		}
};

// -----------------------------------------------------------------------------

// Stores information related to a surface scattering interaction
struct SurfaceInteraction : Interaction
{
	// Reference to the geom
	const CPUGeometry* geometry = nullptr;

	/// UV surface coordinates
	Vec2f uv = Vec2f(zero);

	/// Shading frame
	LinearSpace3f shadingFrame;

	/// Incident direction in the local shading frame
	Vec3f wi = Vec3f(zero);

	unsigned int geomID = SPINDULYS_INVALID_GEOMETRY_ID;
	unsigned int primID = SPINDULYS_INVALID_GEOMETRY_ID;
	unsigned int instID = SPINDULYS_INVALID_GEOMETRY_ID;

	/// Stores a pointer to the parent instance (if applicable)
	const CPUGeometry* instance = nullptr;

	/**
	 * Boundary-test value used in reparameterized integrators, a soft indicator
	 * function which returns a zero value at the silhouette of the shape from
	 * the perspective of a given ray. Everywhere else this function will return
	 * non-negative values reflecting the distance of the surface interaction to
	 * this closest point on the silhouette.
	 */
	float boundaryTest = 0.f;

	void InitializeShadingFrame()
	{
		shadingFrame = frame(shadingFrame.vz);
	}
};

// -----------------------------------------------------------------------------

// Stores information related to a medium scattering interaction
// TODO:
struct MediumInteraction : Interaction
{
	/// Pointer to the associated medium
	// MediumPtr medium = nullptr;

	/// Shading frame
	// Frame3f sh_frame;

	/// Incident direction in the local shading frame
	// Vec3f wi;

	// UnpolarizedSpectrum sigma_s, sigma_n, sigma_t, combined_extinction;

	/// mint used when sampling the given distance "t".
	// float mint;

	/// Convert a local shading-space vector into world space
	// Vec3f to_world(const Vector3f &v) const {
	// return sh_frame.to_world(v);
	// }

	/// Convert a world-space vector into local shading coordinates
	// Vec3f to_local(const Vector3f &v) const {
	// return sh_frame.to_local(v);
	// }
};

// -----------------------------------------------------------------------------

/**
 * \brief Stores preliminary information related to a ray intersection
 *
 * This data structure is used as return type for the \ref Shape::ray_intersect_preliminary
 * efficient ray intersection routine. It stores whether the shape is intersected by a
 * given ray, and cache preliminary information about the intersection if that is the case.
 *
 * If the intersection is deemed relevant, detailed intersection information can later be
 * obtained via the  \ref create_surface_interaction() method.
 */
struct PreliminaryIntersection
{
	// Distance traveled along the ray
	float t = Infinity<float>;

	// 2D coordinates on the primitive surface parameterization
	Vec2f primUV;

	// Primitive index, e.g. the triangle ID (if applicable)
	unsigned int primIndex;

	// Shape index, e.g. the shape ID in shapegroup (if applicable)
	unsigned int shapeIndex;

	// Pointer to the associated shape
	const CPUGeometry* shape = nullptr;

	/// Stores a pointer to the parent instance (if applicable)
	const CPUGeometry* instance = nullptr;

	/// Is the current interaction valid?
	bool isValid() const
	{
		return t != Infinity<float>;
	}

	/**
	 * \brief Compute and return detailed information related to a surface interaction
	 *
	 * \param ray
	 *      Ray associated with the ray intersection
	 * \param ray_flags
	 *      Flags specifying which information should be computed
	 * \return
	 *      A data structure containing the detailed information
	 */
	SurfaceInteraction ComputeSurfaceInteraction(const Ray& ray, uint32_t rayFlags = (uint32_t) RayFlags::All, bool active = true) const
	{
		active &= isValid();
		if (!active)
		{
			SurfaceInteraction si;
			si.wi = -ray.direction;
			return si;
		}

		const CPUGeometry* target = instance ? instance : shape;
		SurfaceInteraction si = target->ComputeSurfaceInteraction(ray, *this, rayFlags, 0u, active);

		si.t = active ? si.t : Infinity<float>;
		active &= si.IsValid();

		if (!active)
		{
			si.geometry = nullptr;
			si.instance = nullptr;
		}

		si.primID = primIndex;
		si.time   = ray.time;

		if (rayFlags & (uint32_t) RayFlags::ShadingFrame)
			si.InitializeShadingFrame();

		// Incident direction in local coordinates
		si.wi = select(active, toLocal(si.shadingFrame, -ray.direction), -ray.direction);

		if (rayFlags & (uint32_t) RayFlags::BoundaryTest)
			si.boundaryTest = active ? si.boundaryTest : 1e8f;

		return si;
	}
};

// -----------------------------------------------------------------------------
// TODO:

static std::ostream &operator<<(std::ostream &os, const Interaction& iteraction)
{
	return os << "{ "
		<< "t = " << iteraction.t
		<< ", time = " << iteraction.time
		<< ", p = " << iteraction.p
		<< ", n = " << iteraction.n
		<< "}";
}

static std::ostream &operator<<(std::ostream &os, const SurfaceInteraction& surfaceInteraction)
{
	return os << "{ "
		<< "t = " << surfaceInteraction.t
		<< ", time = " << surfaceInteraction.time
		<< ", p = " << surfaceInteraction.p
		<< ", n = " << surfaceInteraction.n
		// TODO: Add print statements for each shape << ", geometry = " << surfaceInteraction.geometry
		<< ", uv = " << surfaceInteraction.uv
		<< ", shadingFrame = " << surfaceInteraction.shadingFrame
		<< ", wi = " << surfaceInteraction.wi
		<< ", geomID = " << surfaceInteraction.geomID
		<< ", primID = " << surfaceInteraction.primID
		<< ", instID = " << surfaceInteraction.instID
		// TODO: Add print statements for each shape << ", instance = " << surfaceInteraction.instance
		<< ", boundaryTest = " << surfaceInteraction.boundaryTest
		<< "}";
}

static std::ostream &operator<<(std::ostream &os, const PreliminaryIntersection& pi)
{
	return os << "{ "
	<< "t = " << pi.t << "," << std::endl
	<< ", primUV = " << pi.primUV
	<< ", primIndex = " << pi.primIndex
	<< ", shapeIndex = " << pi.shapeIndex
	// TODO: << ", shape = " << string::indent(pi.shape, 6) << "," << std::endl
	// TODO: << ", instance = " << string::indent(pi.instance, 6) << "," << std::endl
	<< "}";
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_INTERACTION_H
