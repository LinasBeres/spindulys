#include "window.h"

#include <functional>

#include <nfd.h>

#include "output_helper.h"


GUI_NAMESPACE_OPEN_SCOPE


Window::Window()
{
	GUI_TRACE();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
}

int Window::RenderWindow(const std::string& scenePath)
{
	GUI_TRACE();
	SDL_Window* window = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		spdlog::critical("[ERROR] %s\n", SDL_GetError());
		return -1;
	}

	window = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, kDefaultWidth, kDefaultHeight, SDL_WINDOW_SHOWN );
	if( window == NULL )
	{
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		return -1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(
			window,
			-1,
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
			);
	if (renderer == NULL)
		exit(3);

	CPURenderManager renderManager;

	renderManager.LoadScene(scenePath);
	renderManager.GetCamera().SetResolution(Vec2f(renderGlobals.GetWidth(), renderGlobals.GetHeight()));

	RenderManager::StopRenderer stopRenderingFunction = std::bind(&Window::CloseWindow, this);
	renderManager.SetStopRendererCallback(stopRenderingFunction);

	// RenderManager::RegisterUpdates updateRendererFunction =
		// std::bind(&Window::PreRenderCallback, this, std::placeholders::_1);
	// renderManager.SetUpdateCallback(updateRendererFunction);

	// RenderManager::DrawBuffer drawBufferFunction =
		// std::bind(&Window::RenderToScreenTexture, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	// renderManager.SetBufferCallback(drawBufferFunction);

	SetupScreenQuad(renderGlobals.GetWidth(), renderGlobals.GetHeight());

	renderManager.Trace(1, 0, renderGlobals.GetHeight());

	renderManager.Render();

	Buffer3f buffer = renderManager.GetBuffer();

	SDL_Event   ev;
	int         window_running = 1;

	uint32_t* m_main_buffer = new uint32_t[renderGlobals.GetHeight() * renderGlobals.GetWidth()];
	for (uint32_t x = 0; x < renderGlobals.GetWidth(); ++x)
	{
		for (uint32_t y = 0; y < renderGlobals.GetHeight(); ++y)
		{
			uint32_t pixelIdx = x + (y * renderGlobals.GetWidth());
			uint32_t colour(0);

			Col3f colourVec = buffer.GetPixel(pixelIdx);
			uint8_t red = floor(colourVec.r >= 1.0 ? 255 : colourVec.r * 256.0);
			uint8_t blue = floor(colourVec.b >= 1.0 ? 255 : colourVec.b * 256.0);
			uint8_t green = floor(colourVec.g >= 1.0 ? 255 : colourVec.g * 256.0);

			// Bit shift colors into proper positions within color (RGBA)
			colour += red;
			colour <<= 8;
			colour += green;
			colour <<= 8;
			colour += blue;
			colour <<= 8;
			colour += 0xFF;   // Alpha channel set to opaque

			m_main_buffer[pixelIdx] = colour;

		}
	}

	SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, renderGlobals.GetWidth(), renderGlobals.GetHeight());
	SDL_UpdateTexture(texture, NULL, m_main_buffer, sizeof(uint32_t) * renderGlobals.GetWidth());
	SDL_RenderCopy(
			renderer,    // The renderer to be updated.
			texture,     // The source texture.
			NULL,          // Amount of texture to be copied, NULL for entire texture.
			NULL           // Amount of renderer to be updated, NULL for entire renderer.
			);

	while (window_running)
	{
		while (SDL_PollEvent(&ev) != 0)
		{
			if (ev.type == SDL_QUIT)
				window_running = 0;
		}
		SDL_RenderPresent(renderer);
		// SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		// SDL_RenderClear(renderer);
		// SDL_RenderCopy(renderer, texture, NULL, NULL);
		// SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool Window::PreRenderCallback(RenderManager* renderManager)
{
	GUI_TRACE();
	// float currentFrame(glfwGetTime());
	// deltaTime = currentFrame - lastFrame;
	// lastFrame = currentFrame;

	ImGuiIO& guiIO = ImGui::GetIO();
	(void)guiIO;

	SetupGUI(renderManager);
	KeyboardCallback(guiIO, renderManager);

	Vec2f mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	if (prevMousePos.x != mousePos.x || prevMousePos.y != mousePos.y)
		MouseCallback(guiIO, mousePos, renderManager);

	if (renderManager->SetMaxSamples(renderGlobals.GetMaxSamples()))
		renderManager->SetRenderDirty();
	if (renderManager->SetIntegrator(renderGlobals.GetIntegrator()))
		renderManager->SetRenderDirty();
	if (renderManager->SetSampler(renderGlobals.GetSampler()))
		renderManager->SetRenderDirty();
	if (renderManager->SetHideLights(renderGlobals.GetHideLights()))
		renderManager->SetRenderDirty();
	if (renderManager->SetMaxLightSamples(renderGlobals.GetMaxLightsSamples()))
		renderManager->SetRenderDirty();
	if (renderManager->SetMaxBSDFSamples(renderGlobals.GetMaxBSDFSamples()))
		renderManager->SetRenderDirty();
	if (renderManager->SetMaxDepth(renderGlobals.GetMaxDepth()))
		renderManager->SetRenderDirty();
	if (renderManager->SetRussianRouletteDepth(renderGlobals.GetRussianRouletteDepth()))
		renderManager->SetRenderDirty();

	if (renderManager->SetCurrentCamera(sceneCamera))
		renderManager->SetRenderDirty();

	renderManager->SetCurrentBuffer(renderGlobals.GetBufferID());
	renderManager->SetMaxIterations(renderGlobals.GetMaxIterations());

	return true;
}

void Window::SetupGUI(RenderManager* renderManager)
{
	GUI_TRACE();
	ImGui_ImplOpenGL3_NewFrame();
	// ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (profilingState)
		ProfilingWindow(profilingState, renderManager);
	if (aboutState)
		AboutWindow(aboutState);
	if (renderConfigState)
		RenderConfigWindow(renderConfigState);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Export to..."))
			{
				if (ImGui::MenuItem("PPM"))
				{
					toPPM(renderGlobals.GetWidth(),
							renderGlobals.GetHeight(),
							renderManager->GetBuffer());
				}

				if (ImGui::MenuItem("EXR"))
				{
					toEXR(renderGlobals.GetWidth(),
							renderGlobals.GetHeight(),
							renderManager->GetBuffer());
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Rendering"))
		{
			if (ImGui::BeginMenu("Integrator"))
			{
				ImGui::RadioButton("Direct", reinterpret_cast<int *>(&renderGlobals.m_integratorID), 0);
				ImGui::RadioButton("ForwardPath", reinterpret_cast<int *>(&renderGlobals.m_integratorID), 1);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Buffer"))
			{
				ImGui::RadioButton("Beauty", reinterpret_cast<int *>(&renderGlobals.m_bufferID), 0);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Sampler"))
			{
				ImGui::RadioButton("Independent", reinterpret_cast<int *>(&renderGlobals.m_samplerId), 0);
				ImGui::RadioButton("Uniform", reinterpret_cast<int *>(&renderGlobals.m_samplerId), 1);
				ImGui::RadioButton("Stratified", reinterpret_cast<int *>(&renderGlobals.m_samplerId), 2);

				ImGui::EndMenu();
			}

			ImGui::Separator();
			ImGui::MenuItem("Config", NULL, &renderConfigState);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::BeginMenu("Cameras"))
			{
				int i = 0;
				for (const auto& camera : renderManager->GetScene()->GetSceneCameras())
					ImGui::RadioButton(camera.c_str(), &sceneCamera, i++);

				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("Load..."))
			{
				if (const std::string filepath = GetBrowserFilePath(); !filepath.empty())
				{
					renderManager->LoadScene(filepath);
					renderManager->SetRenderDirty();
				}
			}
			if (ImGui::MenuItem("Import..."))
			{
				if (const std::string filepath = GetBrowserFilePath(); !filepath.empty())
					if (renderManager->ImportScene(filepath))
						renderManager->SetRenderDirty();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("Profiling", NULL, &profilingState);
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &aboutState);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

std::string Window::GetBrowserFilePath() const
{
	GUI_TRACE();
	std::string filepath;

	std::string formats = std::string(RenderManager::ValidSceneFormats());

	NFD_Init();
	nfdfilteritem_t filterItem[1] = { { "Scene file", formats.c_str() }};
	nfdchar_t *outPath;
	nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
	if (result == NFD_OKAY)
	{
		filepath = std::string(outPath);
		NFD_FreePath(outPath);
	}
	NFD_Quit();

	return filepath;
}

void Window::RenderGUI()
{
	GUI_TRACE();
	ImGui::Render();
}

void Window::StopGUI()
{
	GUI_TRACE();
	ImGui_ImplOpenGL3_Shutdown();
	// ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Window::RenderConfigWindow(bool& guiOpen)
{
	GUI_TRACE();
	ImGui::Begin("Render Config", &guiOpen);

	ImGui::InputInt("Width", reinterpret_cast<int*>(&renderGlobals.m_width));
	ImGui::InputInt("Height", reinterpret_cast<int*>(&renderGlobals.m_height));

	ImGui::Separator();

	ImGui::InputInt("Max Iterations", reinterpret_cast<int*>(&renderGlobals.m_maxIterations));
	ImGui::InputInt("Samples", reinterpret_cast<int*>(&renderGlobals.m_maxSamplesPerPixel));

	ImGui::Checkbox("Hide Lights", &renderGlobals.m_hideLights);

	ImGui::Separator();

	ImGui::InputInt("Max Ligh Samples", reinterpret_cast<int*>(&renderGlobals.m_maxLightSamples));
	ImGui::InputInt("Max BSDF Samples", reinterpret_cast<int*>(&renderGlobals.m_maxBSDFSamples));

	ImGui::Separator();

	ImGui::InputInt("Max Depth", reinterpret_cast<int*>(&renderGlobals.m_maxDepth));
	ImGui::InputInt("Russian Roulette Depth", reinterpret_cast<int*>(&renderGlobals.m_russianRouletteDepth));

	ImGui::Separator();


	ImGui::End();
}

void Window::ProfilingWindow(bool& guiOpen, RenderManager* renderManager)
{
	GUI_TRACE();
	ImGui::Begin("Profiling", &guiOpen, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Total Samples (iterations * samples): %d", renderManager->GetIterations() * renderManager->GetSamples());
	ImGui::Text("Framerate: %.2f FPS / %.2f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)",
			renderManager->GetScene()->GetSceneCamera().GetPosition().x,
			renderManager->GetScene()->GetSceneCamera().GetPosition().y,
			renderManager->GetScene()->GetSceneCamera().GetPosition().z);

	ImGui::End();
}

void Window::AboutWindow(bool& guiOpen)
{
	GUI_TRACE();
	ImGui::Begin("About", &guiOpen);

	ImGui::Text("Spindulys by Linas Beresna\n\nEmail: linas_beresna@sfu.ca");

	ImGui::End();
}

void Window::KeyboardCallback(ImGuiIO& guiIO, RenderManager* renderManager)
{
	GUI_TRACE();
	// if (guiIO.KeysDown[GLFW_KEY_ESCAPE])
	// {
	// // TODO:
	// }
	// if (guiIO.KeysDown[GLFW_KEY_W])
	// {
	// renderManager->GetCamera().KeyboardCallback(Camera::Forward, deltaTime);
	// renderManager->SetRenderDirty();
	// }
	// if (guiIO.KeysDown[GLFW_KEY_S])
	// {
	// renderManager->GetCamera().KeyboardCallback(Camera::Backward, deltaTime);
	// renderManager->SetRenderDirty();
	// }
	// if (guiIO.KeysDown[GLFW_KEY_A])
	// {
	// renderManager->GetCamera().KeyboardCallback(Camera::Left, deltaTime);
	// renderManager->SetRenderDirty();
	// }
	// if (guiIO.KeysDown[GLFW_KEY_D])
	// {
	// renderManager->GetCamera().KeyboardCallback(Camera::Right, deltaTime);
	// renderManager->SetRenderDirty();
	// }
	//
	// if (guiIO.KeysDown[GLFW_KEY_KP_ADD])
	// {
	// // renderManager->mainCamera->SetAperatureRadius(renderManager->mainCamera->GetAperatureRadius() + 0.005f);
	// renderManager->SetRenderDirty();
	// }
	// if (guiIO.KeysDown[GLFW_KEY_KP_SUBTRACT])
	// {
	// // renderManager->mainCamera->SetAperatureRadius(renderManager->mainCamera->GetAperatureRadius() - 0.005f);
	// renderManager->SetRenderDirty();
	// }
}

void Window::MouseCallback(ImGuiIO& guiIO, Vec2f mousePos,RenderManager* renderManager)
{
	GUI_TRACE();
	if (firstMouse)
	{
		prevMousePos = Vec2f(mousePos);

		firstMouse = false;
	}

	const Vec2f mouseOffset(mousePos.x - prevMousePos.x, mousePos.y - prevMousePos.y);
	prevMousePos = Vec2f(mousePos);

	// if (guiIO.MouseDown[GLFW_MOUSE_BUTTON_RIGHT])
	// {
	// if (mouseOffset != Vec2f())
	// {
	// renderManager->GetCamera().MouseCallback(mouseOffset);
	// renderManager->SetRenderDirty();
	// }
	// }
}

void Window::RenderToScreenTexture(int width, int height, const Buffer3f& buffer)
{
	GUI_TRACE();
	RenderGUI();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::SetupScreenQuad(int width, int height)
{
	GUI_TRACE();
}

void Window::DrawScreenQuad()
{
	GUI_TRACE();
}

GUI_NAMESPACE_CLOSE_SCOPE
