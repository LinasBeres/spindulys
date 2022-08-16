#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include <spindulys/sampler.h>
#include <spindulys/buffer.h>

#include "../spindulysFrontend.h"

#include "../camera/camera.h"
#include "../scene/scene.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class RenderManager
{
	public:
		using StopRenderer    = std::function<bool(void)>;
		using RegisterUpdates = std::function<bool(RenderManager*)>;
		using DrawBuffer      = std::function<void(int, int, const Buffer3f&)>;

		enum class IntegratorIds {
			UDPT = 0,
			Diffuse,
			Occlusion,
			Position,
			Normal,
			Debug,
		};

		enum class BufferIds {
			Beauty = 0,
			Diffuse,
			Position,
			Normal,
			Debug,
		};

		using Buffers = std::unordered_map<BufferIds, std::unique_ptr<Buffer3f>>;

		struct RenderGlobals
		{
			int width = 800;                                   // The width of the image to render.
			int height = 600;                                  // The height of the image to render.
			int maxIterations = 32;                            // The maximum number of times to refine.
			int depth = 3;                                     // The maximum ray depth, or number of bounces, the renderer can make use of.
			int samples = 1;                                   // Total number of samples per pixel to compute.
			IntegratorIds integratorID = IntegratorIds::UDPT;  // The ID of the integrator currently being used by the renderer.
			BufferIds bufferID = BufferIds::Beauty;            // The current buffer being read.
			std::unordered_set<BufferIds> currentBufferIds =   // Available buffers to read.
				{ BufferIds::Beauty, BufferIds::Diffuse, BufferIds::Position, BufferIds::Normal, BufferIds::Debug };
		};

		RenderManager();
		virtual ~RenderManager();

		bool LoadScene(const std::string& filepath) { return scene->LoadScene(filepath); }

		void SetStopRendererCallback(StopRenderer stopFunction) { stopRendererFunction = stopFunction; }
		void SetBufferCallback(DrawBuffer drawFunction)         { drawBufferFunction = drawFunction; }
		void SetUpdateCallback(RegisterUpdates updateFunction)  { updateRendererFunction = updateFunction; }

		void SetRenderDirty() { update = true; }
		bool RenderDirty() const { return (update || scene->SceneDirty()); }

		void Render();
		void ResetRender();
		virtual void Trace(int iterations) = 0;

		// Set Methods - return true if the class parameter was changed.
		bool SetMaxIterations(int maxIterations)       { return maxIterations != std::exchange(renderGlobals.maxIterations, maxIterations); }
		bool SetDepth(int depth)                       { return depth         != std::exchange(renderGlobals.depth, depth);                 }
		bool SetSamples(int samples)                   { return samples       != std::exchange(renderGlobals.samples, samples);             }
		bool SetIntegrator(IntegratorIds integratorID) { return integratorID  != std::exchange(renderGlobals.integratorID, integratorID);   }
		bool SetCurrentBuffer(BufferIds bufferID)      { return bufferID      != std::exchange(renderGlobals.bufferID, bufferID);           } // Note that the render does NOT need to be reset after this.

		// Unique Add method that also returns true if class parameter was changed
		bool AddBuffer(BufferIds bufferID);

		// Get Methods
		int             GetMaxIterations() const { return renderGlobals.maxIterations;                 }
		int             GetDepth()         const { return renderGlobals.depth;                         }
		int             GetSamples()       const { return renderGlobals.samples;                       }
		IntegratorIds   GetIntegrator()    const { return renderGlobals.integratorID;                  }
		const Buffer3f& GetBuffer()        const { return *(buffers.at(renderGlobals.bufferID).get()); }
		const Buffers&  GetBuffers()       const { return buffers;                                     }

		std::unique_ptr<Camera> mainCamera;

	protected:
		RenderGlobals renderGlobals;

		Buffer3f buffer = Buffer3f(renderGlobals.width, renderGlobals.height);
		Buffers buffers;

		Scene* scene = nullptr;

		bool update = false;

		StopRenderer stopRendererFunction = [] { return false; };
		RegisterUpdates updateRendererFunction;
		DrawBuffer drawBufferFunction;

		int iterations = 0;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_MANAGER_H
