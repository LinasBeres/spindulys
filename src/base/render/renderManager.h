#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <string_view>

#include <spindulys/sampler.h>
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
		using RegisterUpdates = std::function<bool(RenderManager*)>;
		using DrawBuffer = std::function<void(int, int, const Buffer3f&)>;

		using Buffers = std::unordered_map<BufferIds, Buffer3f*>;

		RenderManager();
		virtual ~RenderManager();

		bool ImportScene(const std::string& filepath);
		void LoadScene(const std::string& filepath);
		// TODO: Find out a way to make this work with constexpr.
		static const std::string_view ValidSceneFormats();

		void SetStopRendererCallback(StopRenderer stopFunction) { stopRendererFunction = stopFunction; }
		void SetBufferCallback(DrawBuffer drawFunction)         { drawBufferFunction = drawFunction; }
		void SetUpdateCallback(RegisterUpdates updateFunction)  { updateRendererFunction = updateFunction; }

		void SetRenderDirty() { update = true; }
		bool RenderDirty() const { return (update || scene->SceneDirty()); }

		void Render();
		void ResetRender();
		virtual void Trace(int iterations) = 0;

		// Set Methods - return true if the class parameter was changed.
		bool SetMaxIterations(uint32_t maxIterations)  { return renderGlobals.SetMaxIterations(maxIterations); }

		bool SetMaxDepth(uint32_t maxDepth)            { return renderGlobals.SetMaxDepth(maxDepth);           }
		bool SetMaxSamples(uint32_t maxSamples)        { return renderGlobals.SetMaxSamples(maxSamples);       }

		// Note that the render does NOT need to be reset after this.
		bool SetCurrentBuffer(BufferIds bufferID)      { return renderGlobals.SetCurrentBuffer(bufferID);      }

		bool SetScaleResolution(bool scaleResolution)  { return renderGlobals.SetScaleResolution(scaleResolution); }

		// Variant render manager will also set the correct integrator pointer.
		virtual bool SetIntegrator(IntegratorIds integratorID) { return renderGlobals.SetIntegrator(integratorID); }

		bool SetCurrentCamera(size_t cameraId)         { return scene->SetSceneCamera(cameraId); }

		// Unique Add/Remove method that also returns true if class parameter was changed
		bool AddBuffer(BufferIds bufferID);
		bool RemoveBuffer(BufferIds bufferID);

		// Get Methods
		int             GetIterations()      const { return iterations;                                  }
		int             GetMaxIterations()   const { return renderGlobals.GetMaxIterations();            }
		int             GetDepth()           const { return renderGlobals.GetMaxDepth();                 }
		int             GetSamples()         const { return renderGlobals.GetMaxSamples();               }
		IntegratorIds   GetIntegrator()      const { return renderGlobals.GetIntegrator();               }
		bool            GetScaleResolution() const { return renderGlobals.GetScaleResolution();          }
		const Buffer3f& GetBuffer()          const { return *(buffers.at(renderGlobals.GetBufferID()));  }
		const Buffers&  GetBuffers()         const { return buffers;                                     }
		const Scene*    GetScene()           const { return scene;                                       }

		// TODO: Tidy the camera handling.
		Camera& GetCamera() { return scene->UpdateSceneCamera(); }

	protected:
		// Render Info
		uint32_t iterations = 0;
		// TODO: Find better heuristics
		// We start at 25% frame size and then go up from there to 100 in 25% incremements
		float frameSize = 0.f;
		RenderGlobals renderGlobals;

		Vec2i currentResolution = Vec2i(renderGlobals.GetWidth(), renderGlobals.GetHeight());

		Buffers buffers;

		// This abstract class holds the scene geometry itself.
		Scene* scene = nullptr;

		bool update = false;

		// Callback functions.
		StopRenderer stopRendererFunction = [] { return false; };
		RegisterUpdates updateRendererFunction;
		DrawBuffer drawBufferFunction;

	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_MANAGER_H
