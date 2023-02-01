#ifndef CPU_INTGRATOR_H
#define CPU_INTGRATOR_H

#include <spindulys/math/col3.h>

#include <render/renderManager.h>

#include "../spindulysCPU.h"

#include "../scene/cpuScene.h"
#include "../utils/ray.h"

CPU_NAMESPACE_OPEN_SCOPE

class Integrator
{
public:
	Integrator();
	virtual ~Integrator() = default;

	virtual std::pair<Col3f, float>
	Sample(const CPUScene* scene, Sampler* sample, const Ray& ray, Col3f* /* aovs */) const = 0;

	bool SetHideLights(bool hideLights) { return hideLights != std::exchange(m_hideLights, hideLights); }

protected:
	bool m_stop = false;

	bool m_hideLights = false;

private:
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_INTGRATOR_H
