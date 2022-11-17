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

	return { zero, zero };
}


float ForwardPath::MultipleImportantSampleWeight(float pdfA, float pdfB) const
{
	pdfA *= pdfA;
	pdfB *= pdfB;
	const float w = pdfA / (pdfA + pdfB);
	return select(isfinite(w), w, 0.f);
}

CPU_NAMESPACE_CLOSE_SCOPE
