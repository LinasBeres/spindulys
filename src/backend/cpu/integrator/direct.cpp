#include "direct.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

Direct::Direct()
{
	m_weightBSDF = 1.f / (float) m_bsdfSamples;
	m_weightLum  = 1.f / (float) m_lightSamples;

	const size_t sum = m_lightSamples + m_bsdfSamples;
	m_fracBSDF = m_bsdfSamples  / (float) sum;
	m_fracLum  = m_lightSamples / (float) sum;
}

Direct::Direct(size_t lightSamples, size_t bsdfSamples)
	: m_lightSamples(lightSamples), m_bsdfSamples(bsdfSamples)
{
	Direct();
}


std::pair<Col3f, float> Direct::Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */)
{
	Col3f result(zero);

	SurfaceInteraction si = scene->RayIntersect(ray);

	// ----------------------- Visible emitters -----------------------

	if (/* TODO: not hide lights: */ true)
		if (const CPULight* visibleLight = scene->LightHit(si))
			result += visibleLight->Eval(si, true);

	if (!si.IsValid())
		return { result, 0.f };

	// ----------------------- Emitter sampling -----------------------
	// TODO:

	// ------------------------ BSDF sampling -------------------------


	for (size_t i = 0; i < m_bsdfSamples; ++i)
	{
		// auto [bs, bsdf_val] = bsdf->sample(ctx, si, sampler->next_1d(active),
				// sampler->next_2d(active), active);
//
		// bool activeSample = (bsdfValue != 0.f);
//
		// // Trace the ray in the sampled direction and intersect against the scene
		// SurfaceInteraction si_bsdf =
			// scene->RayIntersect(si.SpawnRay(si.to_world(bs.wo)), activeSample);
//
		// // Retain only rays that hit an emitter
		// CPULight* light = si_bsdf.emitter(scene, activeSample);
		// activeSample &= light != nullptr;
//
		// if (activeSample)
		// {
			// Col3f emitterVal = light->Eval(si_bsdf, activeSample);
			// uint32_t delta = has_flag(bs.sampled_type, BSDFFlags::Delta);

			/* Determine probability of having sampled that same
				 direction using Emitter sampling. */
			// DirectionSample ds(scene->LightHit(si), si_bsdf, si);

			// float emitter_pdf = select(delta, 0.f, scene->pdf_emitter_direction(si, ds, active_b));

			// result +=
				// bsdf_val * emitterVal *
				// MultipleImportantSampleWeight(bs.pdf * m_fracBSDF, emitter_pdf * m_fracLum) *
				// m_weightBSDF;
		// }
	}

	// TODO:
	BSDFSample bsdfSample;
	bsdfSample.wi = diffuseMat.Sample2(pixelSample, si, bsdfSample);
	bsdfSample.reflectance = diffuseMat.Evaluate2(pixelSample, si, bsdfSample);

	float directionSign(sign(dot(bsdfSample.wi, si.shadingFrame.vz)));
	Vec3f origin = si.p + (directionSign * 32.0f * 1.19209e-07f * si.shadingFrame.vz);
	Vec3f direction = bsdfSample.wi;

	// Initializing the new ray.
	Ray rh = Ray(origin, direction, 32.0f * 1.19209e-07f);
	if (rh.time != 0) { ; }
	const Vec3f color = bsdfSample.reflectance / bsdfSample.pdf;

	result = result + Col3f(color.x, color.y, color.z);

	return { result, 0.f };
}

float Direct::MultipleImportantSampleWeight(float pdfA, float pdfB) const
{
	pdfA *= pdfA;
	pdfB *= pdfB;
	float w = pdfA / (pdfA + pdfB);
	return select(isfinite(w), w, 0.f);
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
