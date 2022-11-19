#include "forwardPath.h"

#include "../bsdf/cpuBSDF.h"

#include "../utils/records.h"


CPU_NAMESPACE_OPEN_SCOPE

ForwardPath::ForwardPath(uint32_t maxDepth, uint32_t russianRouletteDepth)
	: m_maxDepth(maxDepth), m_russianRouletteDepth(russianRouletteDepth)
{
}


std::pair<Col3f, float>
ForwardPath::Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */) const
{

	bool active = true;

	if (unlikely(m_maxDepth == 0))
		return { zero, false };

	Ray continuousRay = ray;
	Col3f throughput = one;
	Col3f result = zero;
	float eta = 1.f;
	uint32_t depth = 0;

	// If m_hide_emitters == false, the environment emitter will be visible
	bool validRay = !m_hideLights && scene->GetEnvironment() != nullptr;

	// Variables caching information from the previous bounce
	Interaction prevSi;
	float prevBSDFPdf = 1.f;
	bool prevBSDFDelta = true;
	BSDFContext bsdfContext;

	while (active)
	{
		SurfaceInteraction si = scene->RayIntersect(continuousRay);

		// ---------------------- Direct emission ----------------------
		if (const CPULight* visibleLight = scene->LightHit(si))
		{
			DirectionSample ds(si, prevSi, visibleLight);
			float lightPdf = 0.f;

			if (!prevBSDFDelta)
				lightPdf = visibleLight->PdfDirection(prevSi, ds, !prevBSDFDelta);

			// Compute MIS weight for emitter sample from previous bounce
			float misBSDF = MultipleImportantSampleWeight(prevBSDFPdf, lightPdf);

			result = madd(visibleLight->Eval(si, prevBSDFPdf > 0.f) * misBSDF, throughput, result);
		}

		bool activeNext = (depth + 1 < m_maxDepth) && si.IsValid();

		// Early exit if depth exceeded or ray hit is invalid.
		if (!activeNext)
			break;

		// ---------------------- Emitter sampling ----------------------
		const CPUBSDF* bsdf = si.shape->GetBSDF();
		uint32_t flags = bsdf->GetFlags();
		uint32_t activeLight = activeNext && (flags & (uint32_t) BSDFFlags::Smooth) != 0;

		if (activeLight)
		{
			DirectionSample ds;
			Col3f lightVal;

			// Sample light
			std::tie(ds, lightVal) = scene->SampleLightDirection(
					si, pixelSample.sampler.Uniform2D(), true, activeLight);
			activeLight &= ds.pdf != 0.f;

			if (activeLight)
			{
				// Query the BSDF for that emitter-sampled direction
				const Vec3f wo = toLocal(si.shadingFrame, ds.d);

				/* Determine BSDF value and probability of having sampled
					 that same direction using BSDF sampling. */
				auto [bsdfVal, bsdfPdf] = bsdf->EvalPdf(bsdfContext, si, wo, activeLight);
				float mis = select(ds.delta, 1.f, MultipleImportantSampleWeight(ds.pdf, bsdfPdf));

				result = madd(bsdfVal * lightVal * mis, throughput, result);
			}
		}

		// ------------------------ BSDF sampling -------------------------
		float sample1 = pixelSample.sampler.Uniform1D();
		Vec2f sample2 = pixelSample.sampler.Uniform2D();
		BSDFSample bs;
		Col3f bsdfValue;
		std::tie(bs, bsdfValue) = bsdf->Sample(bsdfContext, si, sample1, sample2);

		continuousRay = si.SpawnRay(si.shadingFrame * bs.wo);

		// ------ Update loop variables based on current interaction ------
		throughput = throughput * bsdfValue;
		eta *= bs.eta;
		validRay |= active && si.IsValid() && (bs.sampledType & (uint32_t) BSDFFlags::Null) == 0;

		// Information about the current vertex needed by the next iteration
		prevSi = si;
		prevBSDFPdf = bs.pdf;
		prevBSDFDelta = (bs.sampledType & (uint32_t) BSDFFlags::Delta) != 0;

		// -------------------- Stopping criterion ---------------------

		if (si.IsValid())
			++depth;

		float throughputMax = reduce_max(throughput);

		float russianRouletteProb = min(throughputMax * sqr(eta), .95f);
		bool russianRoulleteActive = depth >= m_russianRouletteDepth;
		bool russianRouletteContinue = pixelSample.sampler.Uniform1D() < russianRouletteProb;

		if (russianRoulleteActive)
			throughput *= rcp(russianRouletteProb);

		active = activeNext && (!russianRoulleteActive || russianRouletteContinue) && (throughputMax != 0.f);
	}

	return { validRay ? result : zero, validRay };
}


float ForwardPath::MultipleImportantSampleWeight(float pdfA, float pdfB) const
{
	pdfA *= pdfA;
	pdfB *= pdfB;
	const float w = pdfA / (pdfA + pdfB);
	return select(isfinite(w), w, 0.f);
}

CPU_NAMESPACE_CLOSE_SCOPE
