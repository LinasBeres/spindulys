#ifndef RENDER_MANAGER_H
#define RENDER_MANAGER_H

#include <memory>
#include <vector>

#include <spindulys/sampler.h>
#include <spindulys/buffer.h>

#include "../spindulysFrontend.h"

#include "../camera/camera.h"
#include "../scene/scene.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class RenderManager
{
	public:
		enum IntegratorIds {
			UDPT = 0,
			Diffuse,
			Occlusion,
			Position,
			Normal,
			Debug,
		};

		struct RenderGlobals
		{
			int width = 800;                                   // The width of the image to render.
			int height = 600;                                  // The height of the image to render.
			int depth = 3;                                     // The maximum ray depth, or number of bounces, the renderer can make use of.
			int samples = 1;                                   // Total number of samples per pixel to compute.
			IntegratorIds integratorID = IntegratorIds::UDPT;  // The ID of the integrator currently being used by the renderer.
			bool rayJitter = true;                             // Define whether the camera rays should be jittered or not.
		};

		RenderManager();
		~RenderManager();

		bool LoadScene(const std::string& filepath) { return scene->LoadScene(filepath); }

		bool RenderDirty() const { return (update || scene->SceneDirty()); }

		const Buffer3f& GetBuffer() { return buffer; }

		virtual void Trace(int iterations) = 0;

		std::unique_ptr<Camera> mainCamera;
		RenderGlobals renderGlobals;

	protected:
		Buffer3f buffer = Buffer3f(renderGlobals.width, renderGlobals.height);
		Scene* scene = nullptr;

		bool update = false;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_MANAGER_H
