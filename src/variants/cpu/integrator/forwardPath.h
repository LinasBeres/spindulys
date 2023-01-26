#ifndef CPU_FORWARD_PATH_H
#define CPU_FORWARD_PATH_H

#include <spindulys/math/col3.h>

#include <render/renderManager.h>

#include "../spindulysCPU.h"

#include "integrator.h"

CPU_NAMESPACE_OPEN_SCOPE

class ForwardPath: public Integrator
{
public:
	ForwardPath(uint32_t maxDepth = 3, uint32_t russianRouletteDepth = 5, bool hideLights = false);

	virtual std::pair<Col3f, float>
	Sample(const CPUScene* scene, Sampler& sampler, const Ray& ray, Col3f* /* aovs */) const override;

	bool SetMaxDepth(uint32_t depth)             { return depth != std::exchange(m_maxDepth, depth);             }
	bool SetRussianRouletteDepth(uint32_t depth) { return depth != std::exchange(m_russianRouletteDepth, depth); }

protected:

private:
	float MultipleImportantSampleWeight(float pdfA, float pdfB) const;

private:
	uint32_t m_maxDepth = 2;

	// Depth to begin using russian roulette
	uint32_t m_russianRouletteDepth = 5;
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_INTGRATOR_H
