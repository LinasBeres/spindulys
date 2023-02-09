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
		using UpdateBuffer = std::function<void(const Buffer3f&)>;

		using Buffers = std::unordered_map<BufferIds, Buffer3f*>;

		RenderManager();
		virtual ~RenderManager();

		bool ImportScene(const std::string& filepath);
		void LoadScene(const std::string& filepath);
		// TODO: Find out a way to make this work with constexpr.
		static const std::string_view ValidSceneFormats();

		void SetStopRendererCallback(StopRenderer stopFunction)   { m_stopRendererFunction = stopFunction; }
		void SetBufferCallback(DrawBuffer drawFunction)           { m_drawBufferFunction = drawFunction; }
		void SetBufferUpdateCallback(UpdateBuffer updateFunction) { m_updateBufferFunction = updateFunction; }
		void SetUpdateCallback(RegisterUpdates updateFunction)    { m_updateRendererFunction = updateFunction; }

		void SetRenderDirty() { m_update = true; }
		bool RenderDirty() const { return (m_update || m_scene->SceneDirty()); }

		void Render();
		void ResetRender();
		virtual void Trace(int iterations, size_t heightBegin, size_t heightEnd) = 0;

		// Set Methods - return true if the class parameter was changed.
		bool SetMaxIterations(uint32_t maxIterations)  { return m_renderGlobals.SetMaxIterations(maxIterations);     }
		bool SetMaxSamples(uint32_t maxSamples)        { return m_renderGlobals.SetMaxSamples(maxSamples);           }
		bool SetCurrentBuffer(BufferIds bufferID)      { return m_renderGlobals.SetCurrentBuffer(bufferID);          }
		bool SetScaleResolution(bool scaleResolution)  { return m_renderGlobals.SetScaleResolution(scaleResolution); }
		bool SetGrowSize(float growSize)               { return m_renderGlobals.SetGrowSize(growSize);               }
		bool SetCurrentCamera(size_t cameraId)         { return m_scene->SetSceneCamera(cameraId);                   }

		// Variant render manager will set the correct integrator parts.
		virtual bool SetIntegrator(IntegratorIds integratorID) { return m_renderGlobals.SetIntegrator(integratorID);    }
		virtual bool SetHideLights(bool hideLights)            { return m_renderGlobals.SetHideLights(hideLights);      }
		virtual bool SetMaxLightSamples(uint32_t samples)      { return m_renderGlobals.SetMaxLightSamples(samples);    }
		virtual bool SetMaxBSDFSamples(uint32_t samples)       { return m_renderGlobals.SetMaxBSDFSamples(samples);     }
		virtual bool SetMaxDepth(uint32_t maxDepth)            { return m_renderGlobals.SetMaxDepth(maxDepth);          }
		virtual bool SetRussianRouletteDepth(uint32_t depth)   { return m_renderGlobals.SetRussianRouletteDepth(depth); }

		// Unique Set/Add/Remove method that also returns true if class parameter was changed
		bool SetSampler(SamplerIds samplerID);
		bool AddBuffer(BufferIds bufferID);
		bool RemoveBuffer(BufferIds bufferID);

		// Get Methods
		int             GetIterations()      const { return m_iterations;                                   }
		int             GetMaxIterations()   const { return m_renderGlobals.GetMaxIterations();             }
		int             GetMaxDepth()        const { return m_renderGlobals.GetMaxDepth();                  }
		int             GetSamples()         const { return m_renderGlobals.GetMaxSamples();                }
		int             GetWidth()           const { return m_currentResolution.x;                          }
		int             GetHeight()          const { return m_currentResolution.y;                          }
		IntegratorIds   GetIntegrator()      const { return m_renderGlobals.GetIntegrator();                }
		SamplerIds      GetSampler()         const { return m_renderGlobals.GetSampler();                   }
		bool            GetScaleResolution() const { return m_renderGlobals.GetScaleResolution();           }
		const Buffer3f& GetBuffer()          const { return *(m_buffers.at(m_renderGlobals.GetBufferID())); }
		const Buffers&  GetBuffers()         const { return m_buffers;                                      }
		const Scene*    GetScene()           const { return m_scene;                                        }
		std::mutex&     GetLock()            const { return m_renderMutex;                                  }

		// TODO: Tidy the camera handling.
		Camera& GetCamera() { return m_scene->UpdateSceneCamera(); }

	protected:
		// Render Info
		uint32_t m_iterations = 1;
		// TODO: Find better heuristics
		// We start at 25% frame size and then go up from there to 100 in 25% incremements
		float m_frameSize = 0.f;
		RenderGlobals m_renderGlobals;

		Vec2i m_currentResolution = Vec2i(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight());

		Buffers m_buffers;

		// This abstract class holds the scene geometry itself.
		Scene* m_scene = nullptr;

		std::unique_ptr<Sampler> m_sampler = std::make_unique<IndependentSampler>();

		bool m_update = false;

		// Callback functions.
		StopRenderer m_stopRendererFunction = [] { return false; };
		RegisterUpdates m_updateRendererFunction;
		DrawBuffer m_drawBufferFunction;
		UpdateBuffer m_updateBufferFunction;

		mutable std::mutex m_renderMutex;

	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_MANAGER_H
