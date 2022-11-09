#include "direct.h"

#include "../bsdf/bsdf.h"

#include "../utils/records.h"


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
	: m_lightSamples(lightSamples)
	, m_bsdfSamples(bsdfSamples)
{
	Direct();
}

std::pair<Col3f, float>
Direct::Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */) const
{
	Col3f result(zero);

	SurfaceInteraction si = scene->RayIntersect(ray);

	// ----------------------- Visible emitters -----------------------

	if (!m_hideLights)
		if (const CPULight* visibleLight = scene->LightHit(si))
			result += visibleLight->Eval(si, true);

	if (!si.IsValid())
		return { result, 0.f };

	// ----------------------- Emitter sampling -----------------------
	BSDFContext ctx;
	const BSDF* bsdf = si.shape->GetBSDF();
  uint32_t flags = bsdf->GetFlags();
  uint32_t sampleLight = (flags & (uint32_t) BSDFFlags::Smooth) != 0;

	if (sampleLight)
	{
		for (size_t i = 0; i < m_lightSamples; ++i)
		{
			uint32_t activeLight = sampleLight;
			DirectionSample ds;
			Col3f lightVal;

			std::tie(ds, lightVal) = scene->SampleLightDirection(
					si, pixelSample.sampler.Uniform2D(), true, activeLight);

			activeLight &= ds.pdf != 0.f;
			if (!activeLight)
				continue;

			// Query the BSDF for that emitter-sampled direction
			const Vec3f wo = toLocal(si.shadingFrame, ds.d);

			/* Determine BSDF value and probability of having sampled
				 that same direction using BSDF sampling. */
			const auto [bsdfVal, bsdfPdf] = bsdf->EvalPdf(ctx, si, wo, activeLight);
			const float mis = select(ds.delta, 1.f, MultipleImportantSampleWeight(
						ds.pdf * m_fracLum, bsdfPdf * m_fracBSDF) * m_weightLum);

			result += mis * bsdfVal * lightVal;
		}
	}

	// ------------------------ BSDF sampling -------------------------
	for (size_t i = 0; i < m_bsdfSamples; ++i)
	{
		// break;
		const auto [bs, bsdfValue] = bsdf->Sample(ctx, si, pixelSample.sampler.Uniform1D(), pixelSample.sampler.Uniform2D());

		// Trace the ray in the sampled direction and intersect against the scene
		SurfaceInteraction si_bsdf = scene->RayIntersect(si.SpawnRay(si.shadingFrame * bs.wo));

		// Retain only rays that hit an emitter
		if (const CPULight* light = scene->LightHit(si_bsdf))
		{
			const Col3f lightVal = light->Eval(si_bsdf, true);

			DirectionSample ds(si, si_bsdf, light);

			const float lightPDF = light->PdfDirection(si, ds, true);

			result += bsdfValue * lightVal *
				MultipleImportantSampleWeight(bs.pdf * m_fracBSDF, lightPDF * m_fracLum) *
				m_weightBSDF;

		}
	}

	// result = Col3f(si.n.x, si.n.y, si.n.z);

	return { result, 0.f };
}

float Direct::MultipleImportantSampleWeight(float pdfA, float pdfB) const
{
	pdfA *= pdfA;
	pdfB *= pdfB;
	const float w = pdfA / (pdfA + pdfB);
	return select(isfinite(w), w, 0.f);
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
