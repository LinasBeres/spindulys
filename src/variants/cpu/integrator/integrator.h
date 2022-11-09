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
	Sample(const CPUScene* scene, PixelSample& pixelSample, const Ray& ray, Col3f* /* aovs */) const = 0;

protected:
	bool m_hideLights = false;

private:
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_INTGRATOR_H
