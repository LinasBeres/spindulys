#ifndef CPU_RENDER_MANAGER_H
#define CPU_RENDER_MANAGER_H

#include <memory>
#include <vector>

#include <spindulys/buffer.h>

#include <render/renderManager.h>

#include "../spindulysCPU.h"

#include "../scene/cpuScene.h"

#include "../camera/cpuCamera.h"
#include "../utils/ray.h"

#include "../integrator/integrator.h"


CPU_NAMESPACE_OPEN_SCOPE

class CPURenderManager final : public RenderManager
{
	public:
		CPURenderManager();
		~CPURenderManager() = default;

		virtual void Trace(int iterations) override;

		virtual bool SetIntegrator(IntegratorIds integratorID) override;
		virtual bool SetHideLights(bool hideLights) override { return RenderManager::SetHideLights(hideLights) && integrator->SetHideLights(hideLights); }
		virtual bool SetMaxLightSamples(uint32_t samples) override;
		virtual bool SetMaxBSDFSamples(uint32_t samples) override;
		virtual bool SetMaxDepth(uint32_t depth) override;
		virtual bool SetRussianRouletteDepth(uint32_t depth) override;

	private:
		void InitialiseIntegrator(IntegratorIds integratorID);

	private:
		std::unique_ptr<Integrator> integrator;
};

CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_RENDER_MANAGER_H
