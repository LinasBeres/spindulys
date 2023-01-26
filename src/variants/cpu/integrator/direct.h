#ifndef CPU_DIRECT_H
#define CPU_DIRECT_H

#include "../spindulysCPU.h"

#include "integrator.h"

CPU_NAMESPACE_OPEN_SCOPE

class Direct final : public Integrator
{
public:
	Direct(uint32_t lightSamples = 1, uint32_t bsdfSamples = 1, bool hideLights = false);

	virtual std::pair<Col3f, float>
	Sample(const CPUScene* scene, Sampler& sampler, const Ray& ray, Col3f* /* aovs */) const override;

	bool SetLightSamples(uint32_t samples);
	bool SetBSDFSamples(uint32_t samples);

private:
	float MultipleImportantSampleWeight(float pdfA, float pdfB) const;

	void ComputeWeights();

private:
	uint32_t m_lightSamples = 1;
	uint32_t m_bsdfSamples = 1;

	float m_fracBSDF;
	float m_fracLum;
	float m_weightBSDF;
	float m_weightLum;
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIRECT_H
