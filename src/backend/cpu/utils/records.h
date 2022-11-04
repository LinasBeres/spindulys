#ifndef CPU_RECORDS_H
#define CPU_RECORDS_H

#include <spindulys/math/vec2.h>
#include <spindulys/math/vec3.h>

#include "interaction.h"

#include "../lights/cpuLight.h"

#include "../spindulysBackendCPU.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE


/**
 * \brief Generic sampling record for positions
 *
 * This sampling record is used to implement techniques that draw a position
 * from a point, line, surface, or volume domain in 3D and furthermore provide
 * auxiliary information about the sample.
 *
 * Apart from returning the position and (optionally) the surface normal, the
 * responsible sampling method must annotate the record with the associated
 * probability density and delta.
 */
struct PositionSample
{
	/// Sampled position
	Vec3f p = zero;

	/// Sampled surface normal (if applicable)
	Vec3f n = zero;

	/**
	 * \brief Optional: 2D sample position associated with the record
	 *
	 * In some uses of this record, a sampled position may be associated with
	 * an important 2D quantity, such as the texture coordinates on a triangle
	 * mesh or a position on the aperture of a sensor. When applicable, such
	 * positions are stored in the \c uv attribute.
	 */
	Vec2f uv = zero;

	/// Associated time value
	float time;

	/// Probability density at the sample
	float pdf = zero;

	/// Set if the sample was drawn from a degenerate (Dirac delta) distribution
	uint32_t delta = false;

	/**
	 * \brief Create a position sampling record from a surface intersection
	 *
	 * This is useful to determine the hypothetical sampling density on a
	 * surface after hitting it using standard ray tracing. This happens for
	 * instance in path tracing with multiple importance sampling.
	 */
	PositionSample(const SurfaceInteraction& si)
		: p(si.p), n(si.shadingFrame.vz), uv(si.uv), time(si.time), pdf(0.f),
		delta(false) { }

	/// Basic field constructor
	PositionSample(const Vec3f& p, const Vec3f& n, const Vec2f& uv,
			float time, float pdf, uint32_t delta)
		: p(p), n(n), uv(uv), time(time), pdf(pdf), delta(delta) { }
};

// -----------------------------------------------------------------------------

/**
 * \brief Record for solid-angle based area sampling techniques
 *
 * This data structure is used in techniques that sample positions relative to
 * a fixed reference position in the scene. For instance, <em>direct
 * illumination strategies</em> importance sample the incident radiance
 * received by a given surface location. Mitsuba uses this approach in a wider
 * bidirectional sense: sampling the incident importance due to a sensor also
 * uses the same data structures and strategies, which are referred to as
 * <em>direct sampling</em>.
 *
 * This record inherits all fields from \ref PositionSample and extends it with
 * two useful quantities that are cached so that they don't need to be
 * recomputed: the unit direction and distance from the reference position to
 * the sampled point.
 */
struct DirectionSample : public PositionSample
{
	/// Unit direction from the reference point to the target shape
	Vec3f d = zero;

	/// Distance from the reference point to the target shape
	float dist = zero;

	/**
	 * \brief Optional: pointer to an associated object
	 *
	 * In some uses of this record, sampling a position also involves choosing
	 * one of several objects (shapes, emitters, ..) on which the position
	 * lies. In that case, the \c object attribute stores a pointer to this
	 * object.
	 */
	const CPULight* light = nullptr;

	DirectionSample(const SurfaceInteraction& si, const Interaction& ref, const CPULight* light = nullptr)
		: PositionSample(si), light(light)
	{
		const Vec3f rel = si.p - ref.p;
		dist = length(rel);
		d = select(si.IsValid(), rel / dist, -si.wi);
	}

	// Element-by-element constructor
	DirectionSample(const Vec3f& p, const Vec3f& n, const Vec2f& uv,
			float time, float pdf, uint32_t delta,
			const Vec3f& d, const float& dist, const CPULight* light)
		: PositionSample(p, n, uv, time, pdf, delta), d(d), dist(dist), light(light) { }

	// Construct from a position sample
	DirectionSample(const PositionSample &base) : PositionSample(base) { }

};

// -----------------------------------------------------------------------------

MAYBE_UNUSED static std::ostream &operator<<(std::ostream& os, const PositionSample& ps)
{
	return os << "{ "
		<< "  p = " << ps.p
		<< ", n = " << ps.n
		<< ", uv = " << ps.uv
		<< ", time = " << ps.time
		<< ", pdf = " << ps.pdf
		<< ", delta = " << ps.delta
		<< "}";
}

MAYBE_UNUSED static std::ostream &operator<<(std::ostream& os, const DirectionSample& ds)
{
	return os << "{ "
		<< "  p = " << ds.p
		<< ", n = " << ds.n
		<< ", uv = " << ds.uv
		<< ", time = " << ds.time
		<< ", pdf = " << ds.pdf
		<< ", delta = " << ds.delta
		// TODO: << "  light = " << ds.light
		<< "  d = " << ds.d
		<< "  dist = " << ds.dist
		<< "}";
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_RECORDS_H
