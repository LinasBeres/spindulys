#ifndef CPU_DIRECT_H
#define CPU_DIRECT_H

#include "../spindulysCPU.h"

#include "integrator.h"

CPU_NAMESPACE_OPEN_SCOPE

class Direct final : public Integrator
{
public:
	Direct();
	Direct(size_t emitterSamples, size_t bsdfSamples);

	virtual std::pair<Col3f, float>
	Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */) const override;

private:
	float MultipleImportantSampleWeight(float pdfA, float pdfB) const;

private:
	size_t m_lightSamples = 1;
	size_t m_bsdfSamples = 1;

	float m_fracBSDF;
	float m_fracLum;
	float m_weightBSDF;
	float m_weightLum;
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIRECT_H
