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
Direct::Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */)
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
	BSDFContext ctx;
	const BSDF* bsdf = si.shape->GetBSDF();

	// ------------------------ BSDF sampling -------------------------


	for (size_t i = 0; i < m_bsdfSamples; ++i)
	{
		auto [bs, bsdfValue] = bsdf->Sample(ctx, si, pixelSample.sampler.Uniform1D(), pixelSample.sampler.Uniform2D());

		// Trace the ray in the sampled direction and intersect against the scene
		SurfaceInteraction si_bsdf = scene->RayIntersect(si.SpawnRay(toWorld(si.shadingFrame, bs.wo)));

		// // Retain only rays that hit an emitter
		const CPULight* light = scene->LightHit(si_bsdf);
		if (!si.IsValid() && light)
		{
			std::cerr << "HIT LIGHT...\n";
			const Col3f lightVal = light->Eval(si_bsdf, true);

			DirectionSample ds(si, si_bsdf, light);

			const float lightPDF = light->PdfDirection(si, ds, true);

			result += bsdfValue * lightVal / lightPDF;

		}
	}

	// BSDFSample bsdfSample;
	// bsdfSample.wi = diffuseMat.Sample2(pixelSample, si, bsdfSample);
	// bsdfSample.reflectance = diffuseMat.Evaluate2(pixelSample, si, bsdfSample);
	//
	// float directionSign(sign(dot(bsdfSample.wi, si.shadingFrame.vz)));
	// Vec3f origin = si.p + (directionSign * 32.0f * 1.19209e-07f * si.shadingFrame.vz);
	// Vec3f direction = bsdfSample.wi;
	//
	// // Initializing the new ray.
	// Ray rh = Ray(origin, direction, 32.0f * 1.19209e-07f);
	// if (rh.time != 0) { ; }
	// const Vec3f color = bsdfSample.reflectance / bsdfSample.pdf;
	//
	// result = result + Col3f(color.x, color.y, color.z);

	// result = Col3f(si.n.x, si.n.y, si.n.z);
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
