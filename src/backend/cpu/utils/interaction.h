#ifndef CPU_INTERACTION_H
#define CPU_INTERACTION_H

#include <spindulys/math/constants.h>
#include <spindulys/math/vec3.h>

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

	/// Constructor
	Interaction() {}

	Interaction(float t, float time, const Vec3f& p, const Vec3f& n = Vec3f(zero))
		: t(t), time(time), p(p), n(n) { }

	// Is the current interaction valid?
	int is_valid() const
	{
		return t != static_cast<float>(PosInfTy());
	}

	// Spawn a semi-infinite ray towards the given direction
	Ray spawn_ray(const Vec3f& d) const
	{
		return Ray(offset_p(d), d, 0.f, Largest<float>, time);
	}

	/// Spawn a finite ray towards the given position
	Ray spawn_ray_to(const Vec3f& t) const
	{
		Vec3f o = offset_p(t - p);
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
	Vec3f offset_p(const Vec3f& d) const
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
	// Frame3f sh_frame;

	/// Position partials wrt. the UV parameterization
	// Vec3f dp_du, dp_dv;

	/// Normal partials wrt. the UV parameterization
	// Vec3f dn_du, dn_dv;

	/// UV partials wrt. changes in screen-space
	// Vec2f duv_dx, duv_dy;

	/// Incident direction in the local shading frame
	Vec3f wi = Vec3f(zero);

	unsigned int geomID = SPINDULYS_INVALID_GEOMETRY_ID;  // Embree Geometry ID of the object the ray hit.
	unsigned int primID = SPINDULYS_INVALID_GEOMETRY_ID;  // Embree Primitive ID of the object the ray hit.
	unsigned int instID = SPINDULYS_INVALID_GEOMETRY_ID;  // Embree Instance ID of the object the ray hit.

	/// Stores a pointer to the parent instance (if applicable)
	const CPUGeometry* instance = nullptr;

	/**
	 * Boundary-test value used in reparameterized integrators, a soft indicator
	 * function which returns a zero value at the silhouette of the shape from
	 * the perspective of a given ray. Everywhere else this function will return
	 * non-negative values reflecting the distance of the surface interaction to
	 * this closest point on the silhouette.
	 */
	float boundary_test = 0.f;

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
struct PreliminaryIntersection {
	// Distance traveled along the ray
	float t = Infinity<float>;

	// 2D coordinates on the primitive surface parameterization
	Vec2f prim_uv;

	// Primitive index, e.g. the triangle ID (if applicable)
	uint32_t prim_index;

	// Shape index, e.g. the shape ID in shapegroup (if applicable)
	uint32_t shape_index;

	// Pointer to the associated shape
	const CPUGeometry* shape = nullptr;

	/// Stores a pointer to the parent instance (if applicable)
	const CPUGeometry* instance = nullptr;

	/// Is the current interaction valid?
	bool is_valid() const
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
	SurfaceInteraction ComputeSurfaceInteraction(const Ray& ray, uint32_t ray_flags = (uint32_t) RayFlags::All, bool active = true)
	{
		active &= is_valid();
		if (!active)
		{
			SurfaceInteraction si;
			si.wi = -ray.direction;
			return si;
		}

		const CPUGeometry* target = instance ? instance : shape;
		SurfaceInteraction si = target->ComputeSurfaceInteraction(ray, *this, ray_flags, 0u, active);

		// dr::masked(si.t, !active) = dr::Infinity<Float>;
		// active &= si.is_valid();
//
		// dr::masked(si.shape,    !active) = nullptr;
		// dr::masked(si.instance, !active) = nullptr;
//
		// si.prim_index  = prim_index;
		// si.time        = ray.time;
		// si.wavelengths = ray.wavelengths;
//
		// if (has_flag(ray_flags, RayFlags::ShadingFrame))
			// si.initialize_sh_frame();
//
		// // Incident direction in local coordinates
		// si.wi = dr::select(active, si.to_local(-ray.d), -ray.d);
//
		// si.duv_dx = si.duv_dy = dr::zeros<Point2f>();
//
		// if (has_flag(ray_flags, RayFlags::BoundaryTest))
			// si.boundary_test =
				// dr::select(active, dr::detach(si.boundary_test), 1e8f);

		return si;
	}
};

// -----------------------------------------------------------------------------

template <typename Float, typename Spectrum>
std::ostream &operator<<(std::ostream &os, const Interaction<Float, Spectrum> &it) {
	if (dr::none(it.is_valid())) {
		os << "Interaction[invalid]";
	} else {
		os << "Interaction[" << std::endl
			<< "  t = " << it.t << "," << std::endl
			<< "  time = " << it.time << "," << std::endl
			<< "  wavelengths = " << it.wavelengths << "," << std::endl
			<< "  p = " << string::indent(it.p, 6) << std::endl
			<< "]";
	}
	return os;
}

template <typename Float, typename Spectrum>
std::ostream &operator<<(std::ostream &os, const SurfaceInteraction<Float, Spectrum> &it) {
	if (dr::none(it.is_valid())) {
		os << "SurfaceInteraction[invalid]";
	} else {
		os << "SurfaceInteraction[" << std::endl
			<< "  t = " << it.t << "," << std::endl
			<< "  time = " << it.time << "," << std::endl
			<< "  wavelengths = " << string::indent(it.wavelengths, 16) << "," << std::endl
			<< "  p = " << string::indent(it.p, 6) << "," << std::endl
			<< "  shape = " << string::indent(it.shape, 2) << "," << std::endl
			<< "  uv = " << string::indent(it.uv, 7) << "," << std::endl
			<< "  n = " << string::indent(it.n, 6) << "," << std::endl
			<< "  sh_frame = " << string::indent(it.sh_frame, 2) << "," << std::endl
			<< "  dp_du = " << string::indent(it.dp_du, 10) << "," << std::endl
			<< "  dp_dv = " << string::indent(it.dp_dv, 10) << "," << std::endl;

		if (it.has_n_partials())
			os << "  dn_du = " << string::indent(it.dn_du, 11) << "," << std::endl
				<< "  dn_dv = " << string::indent(it.dn_dv, 11) << "," << std::endl;

		if (it.has_uv_partials())
			os << "  duv_dx = " << string::indent(it.duv_dx, 11) << "," << std::endl
				<< "  duv_dy = " << string::indent(it.duv_dy, 11) << "," << std::endl;

		os << "  wi = " << string::indent(it.wi, 7) << "," << std::endl
			<< "  prim_index = " << it.prim_index << "," << std::endl
			<< "  instance = " << string::indent(it.instance, 13) << std::endl
			<< "]";
	}
	return os;
}

template <typename Float, typename Spectrum>
std::ostream &operator<<(std::ostream &os, const MediumInteraction<Float, Spectrum> &it) {
	if (dr::none(it.is_valid())) {
		os << "MediumInteraction[invalid]";
	} else {
		os << "MediumInteraction[" << std::endl
			<< "  t = " << it.t << "," << std::endl
			<< "  time = " << it.time << "," << std::endl
			<< "  wavelengths = " << it.wavelengths << "," << std::endl
			<< "  p = " << string::indent(it.p, 6) << "," << std::endl
			<< "  medium = " << string::indent(it.medium, 2) << "," << std::endl
			<< "  sh_frame = " << string::indent(it.sh_frame, 2) << "," << std::endl
			<< "  wi = " << string::indent(it.wi, 7) << "," << std::endl
			<< "]";
	}
	return os;
}

template <typename Float, typename Shape>
std::ostream &operator<<(std::ostream &os, const PreliminaryIntersection<Float, Shape> &pi) {
	if (dr::none(pi.is_valid())) {
		os << "PreliminaryIntersection[invalid]";
	} else {
		os << "PreliminaryIntersection[" << std::endl
			<< "  t = " << pi.t << "," << std::endl
			<< "  prim_uv = " << pi.prim_uv << "," << std::endl
			<< "  prim_index = " << pi.prim_index << "," << std::endl
			<< "  shape_index = " << pi.shape_index << "," << std::endl
			<< "  shape = " << string::indent(pi.shape, 6) << "," << std::endl
			<< "  instance = " << string::indent(pi.instance, 6) << "," << std::endl
			<< "]";
	}
	return os;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // RAY_H
