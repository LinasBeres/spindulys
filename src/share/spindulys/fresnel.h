#ifndef SPINDULYS_FRESNEL_H
#define SPINDULYS_FRESNEL_H

#include "../spindulys.h"

#include "math/math.h"
#include "math/vec3.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

__forceinline
std::tuple<float, float, float, float>
fresnel(float cos_theta_i, float eta)
{
	bool outside_mask = cos_theta_i >= 0.f;

	float rcp_eta = rcp(eta);
	float eta_it = select(outside_mask, eta, rcp_eta);
	float eta_ti = select(outside_mask, rcp_eta, eta);

	/* Using Snell's law, calculate the squared sine of the
		 angle between the surface normal and the transmitted ray */
	float cos_theta_t_sqr = nmadd(nmadd(cos_theta_i, cos_theta_i, 1.f), eta_ti * eta_ti, 1.f);

	/* Find the absolute cosines of the incident/transmitted rays */
	float cos_theta_i_abs = abs(cos_theta_i);
	float cos_theta_t_abs = safe_sqrt(cos_theta_t_sqr);

	bool index_matched = eta == 1.f;
	bool special_case  = index_matched || cos_theta_i_abs == 0.f;

	float r_sc = select(index_matched, 0.f, 1.f);

	/* Amplitudes of reflected waves */
	float a_s = nmadd(eta_it, cos_theta_t_abs, cos_theta_i_abs) / madd(eta_it, cos_theta_t_abs, cos_theta_i_abs);
	float a_p = nmadd(eta_it, cos_theta_i_abs, cos_theta_t_abs) / madd(eta_it, cos_theta_i_abs, cos_theta_t_abs);

	float r = special_case ? r_sc : 0.5f * (sqr(a_s) + sqr(a_p));

	/* Adjust the sign of the transmitted direction */
	float cos_theta_t = cos_theta_i >= 0 ? -cos_theta_t_abs : cos_theta_t_abs;

	return { r, cos_theta_t, eta_it, eta_ti};
}

__forceinline float fresnel_conductor(float cos_theta_i, float eta_r, float eta_i)
{
	// Modified from "Optics" by K.D. Moeller, University Science Books, 1988
	float cos_theta_i_2 = cos_theta_i * cos_theta_i;
	float sin_theta_i_2 = 1.f - cos_theta_i_2;
	float sin_theta_i_4 = sin_theta_i_2 * sin_theta_i_2;

	float temp_1   = eta_r * eta_r - eta_i * eta_i - sin_theta_i_2;
	float a_2_pb_2 = safe_sqrt(temp_1*temp_1 + 4.f * eta_i * eta_i * eta_r * eta_r);
	float a        = safe_sqrt(.5f * (a_2_pb_2 + temp_1));

	float term_1 = a_2_pb_2 + cos_theta_i_2;
	float term_2 = 2.f * cos_theta_i * a;

	float r_s = (term_1 - term_2) / (term_1 + term_2);

	float term_3 = a_2_pb_2 * cos_theta_i_2 + sin_theta_i_4;
	float term_4 = term_2 * sin_theta_i_2;

	float r_p = r_s * (term_3 - term_4) / (term_3 + term_4);

	return 0.5f * (r_s + r_p);
}

// Reflection in local coordinates
__forceinline Vec3f reflect(const Vec3f& wi) { return Vec3f(-wi.x, -wi.y, wi.z); }

// Reflect with respect to a given surface normal
__forceinline Vec3f reflect(const Vec3f& wi, const Vec3f& m)
{
	return msub(2.f * dot(wi, m), m, wi);
}

/**
 * Refraction in local coordinates
 *
 * The 'cos_theta_t' and 'eta_ti' parameters are given by the last two tuple
 * entries returned by the fresnel and fresnel_polarized functions.
 */
__forceinline Vec3f refract(const Vec3f& wi, float cos_theta_t, float eta_ti)
{
	return Vec3f(-eta_ti * wi.x, -eta_ti * wi.y, cos_theta_t);
}

// Refract wi with respect to a given surface normal
__forceinline Vec3f refract(const Vec3f& wi, const Vec3f& m, float cos_theta_t, float eta_ti)
{
	return msub(madd(dot(wi, m), eta_ti, cos_theta_t), m, wi * eta_ti);
}

/**
 * \brief Computes the diffuse unpolarized Fresnel reflectance of a dielectric
 * material (sometimes referred to as "Fdr").
 *
 * This value quantifies what fraction of diffuse incident illumination
 * will, on average, be reflected at a dielectric material boundary
 */
__forceinline float fresnel_diffuse_reflectance(float eta)
{
	/* Fast mode: the following code approximates the diffuse Frensel reflectance
		 for the eta<1 and eta>1 cases. An evaluation of the accuracy led to the
		 following scheme, which cherry-picks fits from two papers where they are
		 best. */
	float inv_eta = rcp(eta);

	/* Fit by Egan and Hilgeman (1973). Works reasonably well for
		 "normal" IOR values (<2).
		 Max rel. error in 1.0 - 1.5 : 0.1%
		 Max rel. error in 1.5 - 2   : 0.6%
		 Max rel. error in 2.0 - 5   : 9.5%
		 */
	float approx_1 = madd(0.0636f, inv_eta, madd(eta, madd(eta, -1.4399f, 0.7099f), 0.6681f));

	/* Fit by d'Eon and Irving (2011)

		 Maintains a good accuracy even for unrealistic IOR values.

		 Max rel. error in 1.0 - 2.0   : 0.1%
		 Max rel. error in 2.0 - 10.0  : 0.2%
		 */

	// Using Horner's method.
	float coeff[] = { 0.919317f, -3.4793f, 6.75335f, -7.80989f, 4.98554f, -1.36881f};
	float approx_2 = coeff[5];
	for (size_t i = 1; i < 5; ++i)
		approx_2 = madd(inv_eta, approx_2, coeff[5 - i]);

	return select(eta < 1.f, approx_1, approx_2);
}

SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_FRESNEL_H
