#ifndef CPU_RENDER_MANAGER_H
#define CPU_RENDER_MANAGER_H

#include <memory>
#include <vector>

#include <spindulys/sampler.h>
#include <spindulys/buffer.h>

#include <render/renderManager.h>

#include "../spindulysBackendCPU.h"

#include "../scene/cpuScene.h"

#include "../camera/cpuCamera.h"
#include "../utils/ray.h"

#include "../integrator/integrator.h"
#include "../integrator/udpt.h"
// #include "../integrator/diffuse.h"
// #include "../integrator/occlusion.h"
// #include "../integrator/position.h"
// #include "../integrator/normal.h"
// #include "../integrator/debug.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

class CPURenderManager final : public RenderManager
{
	public:
		CPURenderManager();
		~CPURenderManager() = default;

		virtual void Trace(int iterations) override;

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_RENDER_MANAGER_H
