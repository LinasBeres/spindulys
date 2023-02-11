#include "direct.h"

#include <spindulys/fwd.h>

#include "../bsdf/cpuBSDF.h"

#include "../utils/records.h"


CPU_NAMESPACE_OPEN_SCOPE

Direct::Direct(uint32_t lightSamples, uint32_t bsdfSamples, bool hideLights)
	: m_lightSamples(lightSamples)
	, m_bsdfSamples(bsdfSamples)
{
	m_hideLights = hideLights;
	ComputeWeights();
}

void Direct::ComputeWeights()
{
	m_weightBSDF = 1.f / (float) m_bsdfSamples;
	m_weightLum  = 1.f / (float) m_lightSamples;

	const size_t sum = m_lightSamples + m_bsdfSamples;
	m_fracBSDF = m_bsdfSamples  / (float) sum;
	m_fracLum  = m_lightSamples / (float) sum;
}

bool Direct::SetLightSamples(uint32_t samples)
{
	if (samples == std::exchange(m_lightSamples, samples))
		return false;

	ComputeWeights();
	return true;
}

bool Direct::SetBSDFSamples(uint32_t samples)
{
	if (samples == std::exchange(m_bsdfSamples, samples))
		return false;

	ComputeWeights();
	return true;
}

std::pair<Col3f, float>
Direct::Sample(const CPUScene* scene, Sampler* sampler, const Ray& ray, Col3f* /* aovs */) const
{
	Col3f result(zero);

	SurfaceInteraction si = scene->RayIntersect(ray);
	bool validRay = si.IsValid();

	// ----------------------- Visible emitters -----------------------

	if (!m_hideLights)
		if (const CPULight* visibleLight = scene->LightHit(si))
			result += visibleLight->Eval(si, true);

	if (!si.IsValid())
		return { result, validRay };

	// ----------------------- Emitter sampling -----------------------
	BSDFContext ctx;
	const CPUBSDF* bsdf = si.shape->GetBSDF();
  uint32_t flags = bsdf->GetFlags();
  uint32_t sampleLight = HasFlag(flags, BSDFFlags::Smooth);

	if (sampleLight)
	{
		for (size_t i = 0; i < m_lightSamples; ++i)
		{
			uint32_t activeLight = sampleLight;
			DirectionSample ds;
			Col3f lightVal;

			std::tie(ds, lightVal) = scene->SampleLightDirection(
					si, sampler->Next2d(), true, activeLight);

			activeLight &= ds.pdf != 0.f;
			if (!activeLight)
				continue;

			// Query the BSDF for that emitter-sampled direction
			const Vec3f wo = toLocal(si.shadingFrame, ds.d);

			/* Determine BSDF value and probability of having sampled
				 that same direction using BSDF sampling. */
			auto [bsdfVal, bsdfPdf] = bsdf->EvalPdf(ctx, si, wo, activeLight);
			float mis = select(ds.delta, 1.f, MultipleImportantSampleWeight(
						ds.pdf * m_fracLum, bsdfPdf * m_fracBSDF) * m_weightLum);

			result += mis * bsdfVal * lightVal;
		}
	}

	// ------------------------ BSDF sampling -------------------------
	for (size_t i = 0; i < m_bsdfSamples; ++i)
	{
		BSDFSample bs;
		Col3f bsdfValue;
		std::tie(bs, bsdfValue) = bsdf->Sample(ctx, si, sampler->Next1d(), sampler->Next2d());

		// Trace the ray in the sampled direction and intersect against the scene
		SurfaceInteraction si_bsdf = scene->RayIntersect(si.SpawnRay(si.shadingFrame * bs.wo));

		// Retain only rays that hit an emitter
		if (const CPULight* light = scene->LightHit(si_bsdf))
		{
			const Col3f lightVal = light->Eval(si_bsdf, true);

			DirectionSample ds(si, si_bsdf, light);

			const float lightPDF = scene->PdfLightDirection(si, ds, true);
			float mis = MultipleImportantSampleWeight(bs.pdf * m_fracBSDF, lightPDF * m_fracLum);

			result += bsdfValue * lightVal * mis * m_weightBSDF;

		}
	}

	return { result, validRay };
}

float Direct::MultipleImportantSampleWeight(float pdfA, float pdfB) const
{
	pdfA *= pdfA;
	pdfB *= pdfB;
	const float w = pdfA / (pdfA + pdfB);
	return select(isfinite(w), w, 0.f);
}

CPU_NAMESPACE_CLOSE_SCOPE
