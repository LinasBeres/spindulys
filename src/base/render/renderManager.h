#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string_view>

#include <spindulys/samplers/sampler.h>
#include <spindulys/samplers/stratified.h>
#include <spindulys/samplers/independent.h>
#include <spindulys/samplers/uniformDistrabution.h>
#include <spindulys/buffer.h>
#include <spindulys/defaults.h>

#include "../spindulysBase.h"

#include "renderGlobals.h"

#include "../camera/camera.h"
#include "../scene/scene.h"

BASE_NAMESPACE_OPEN_SCOPE

class RenderManager
{
	public:
		using StopRenderer = std::function<bool(void)>;
		using RegisterUpdates = std::function<bool(void)>;
		using DrawBuffer = std::function<void(int, int, const Buffer3f&)>;
		using UpdateBuffer = std::function<void(void)>;

		using Buffers = std::unordered_map<BufferIds, Buffer3f*>;

		RenderManager();
		virtual ~RenderManager();

		bool ImportScene(const std::string& filepath);
		void LoadScene(const std::string& filepath);
		// TODO: Find out a way to make this work with constexpr.
		static const std::string_view ValidSceneFormats();

		void SetStopRendererCallback(StopRenderer stopFunction) { stopRendererFunction = stopFunction; }
		void SetBufferCallback(DrawBuffer drawFunction)         { drawBufferFunction = drawFunction; }
		void SetUpdateCallback(UpdateBuffer updateFunction)     { updateBufferFunction = updateFunction; }
		void SetUpdateCallback(RegisterUpdates updateFunction)  { updateRendererFunction = updateFunction; }

		void SetRenderDirty() { update = true; }
		bool RenderDirty() const { return (update || scene->SceneDirty()); }

		void Render();
		void ResetRender();
		virtual void Trace(int iterations, size_t heightBegin, size_t heightEnd) = 0;

		// Set Methods - return true if the class parameter was changed.
		bool SetMaxIterations(uint32_t maxIterations)  { return renderGlobals.SetMaxIterations(maxIterations);     }
		bool SetMaxSamples(uint32_t maxSamples)        { return renderGlobals.SetMaxSamples(maxSamples);           }
		bool SetCurrentBuffer(BufferIds bufferID)      { return renderGlobals.SetCurrentBuffer(bufferID);          }
		bool SetScaleResolution(bool scaleResolution)  { return renderGlobals.SetScaleResolution(scaleResolution); }
		bool SetGrowSize(float growSize)               { return renderGlobals.SetGrowSize(growSize);               }
		bool SetCurrentCamera(size_t cameraId)         { return scene->SetSceneCamera(cameraId);                   }

		// Variant render manager will set the correct integrator parts.
		virtual bool SetIntegrator(IntegratorIds integratorID) { return renderGlobals.SetIntegrator(integratorID);    }
		virtual bool SetHideLights(bool hideLights)            { return renderGlobals.SetHideLights(hideLights);      }
		virtual bool SetMaxLightSamples(uint32_t samples)      { return renderGlobals.SetMaxLightSamples(samples);    }
		virtual bool SetMaxBSDFSamples(uint32_t samples)       { return renderGlobals.SetMaxBSDFSamples(samples);     }
		virtual bool SetMaxDepth(uint32_t maxDepth)            { return renderGlobals.SetMaxDepth(maxDepth);          }
		virtual bool SetRussianRouletteDepth(uint32_t depth)   { return renderGlobals.SetRussianRouletteDepth(depth); }

		// Unique Set/Add/Remove method that also returns true if class parameter was changed
		bool SetSampler(SamplerIds samplerID);
		bool AddBuffer(BufferIds bufferID);
		bool RemoveBuffer(BufferIds bufferID);

		// Get Methods
		int             GetIterations()      const { return iterations;                                  }
		int             GetMaxIterations()   const { return renderGlobals.GetMaxIterations();            }
		int             GetMaxDepth()        const { return renderGlobals.GetMaxDepth();                 }
		int             GetSamples()         const { return renderGlobals.GetMaxSamples();               }
		int             GetWidth()           const { return currentResolution.x;                         }
		int             GetHeight()          const { return currentResolution.y;                         }
		IntegratorIds   GetIntegrator()      const { return renderGlobals.GetIntegrator();               }
		SamplerIds      GetSampler()         const { return renderGlobals.GetSampler();                  }
		bool            GetScaleResolution() const { return renderGlobals.GetScaleResolution();          }
		const Buffer3f& GetBuffer()          const { return *(buffers.at(renderGlobals.GetBufferID()));  }
		const Buffers&  GetBuffers()         const { return buffers;                                     }
		const Scene*    GetScene()           const { return scene;                                       }
		std::mutex&     GetLock()            const { return m_renderMutex;                               }

		// TODO: Tidy the camera handling.
		Camera& GetCamera() { return scene->UpdateSceneCamera(); }

	protected:
		// Render Info
		uint32_t iterations = 1;
		// TODO: Find better heuristics
		// We start at 25% frame size and then go up from there to 100 in 25% incremements
		float frameSize = 0.f;
		RenderGlobals renderGlobals;

		Vec2i currentResolution = Vec2i(renderGlobals.GetWidth(), renderGlobals.GetHeight());

		Buffers buffers;

		// This abstract class holds the scene geometry itself.
		Scene* scene = nullptr;

		std::unique_ptr<Sampler> sampler = std::make_unique<IndependentSampler>();

		bool update = false;

		// Callback functions.
		StopRenderer stopRendererFunction = [] { return false; };
		RegisterUpdates updateRendererFunction;
		DrawBuffer drawBufferFunction;
		UpdateBuffer updateBufferFunction;

		mutable std::mutex m_renderMutex;

	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_MANAGER_H
