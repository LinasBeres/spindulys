#include "window.h"

#include <functional>
#include <thread>

#include <nfd.h>

#include "output_helper.h"


GUI_NAMESPACE_OPEN_SCOPE


Window::Window()
{
	GUI_TRACE();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	if (!InitGLFW())
		exit(EXIT_FAILURE);
	if (!InitWindow())
		exit(EXIT_FAILURE);
}

bool Window::InitGLFW()
{
	const char* description;
	if (!glfwInit())
	{
		glfwGetError(&description);
		spdlog::critical("Could not init due to {}.\n Exiting.", description);
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return true;
}

bool Window::InitWindow()
{
	const char* description;
	m_window = glfwCreateWindow(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight(), "Spindulys", nullptr, nullptr);
	if (!m_window)
	{
		glfwGetError(&description);
		spdlog::critical("Could not create window due to {}.\n Exiting.", description);
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // Enable vsync

	if (!gladLoadGL())
	{
		glfwGetError(&description);
		spdlog::critical("Could load GL from glad due to {}.\n Exiting.", description);
		exit(EXIT_FAILURE);
	}

	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// On a resize we just adjust the viewport size not the resolution of buffers
	glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); });

	SetupScreenQuad(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight());
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	return true;
}

void Window::RenderWindow(const std::string& scenePath)
{
	GUI_TRACE();

	m_renderManager.LoadScene(scenePath);
	m_renderManager.GetCamera().SetResolution(Vec2f(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight()));

	m_renderManager.SetStopRendererCallback(std::bind(&Window::CloseWindow, this));
	m_renderManager.SetUpdateCallback(std::bind(&Window::PreRenderCallback, this));
	m_renderManager.SetBufferUpdateCallback(std::bind(&Window::UpdateScreen, this, std::placeholders::_1));

	std::thread renderThread(
		[&renderManager = m_renderManager]
		{
			renderManager.Render();
			spdlog::info("Render thread closing.");
		}
	);

	while(!CloseWindow())
	{
		glfwWaitEvents();

		SetupGUI();
		KeyboardCallback();
		MouseCallback();

		if (m_updateScreen)
		{
			const std::lock_guard<std::mutex> lock(m_renderManager.GetLock());
			RenderToScreenTexture(m_renderManager.GetWidth(), m_renderGlobals.GetHeight(), m_currentBuffer);
			m_updateScreen = false;
		}
		else
		{
			DrawScreenQuad();
		}

		RenderGUI();
		glfwSwapBuffers(m_window);
	}

	renderThread.join();
	StopGUI();
	glfwTerminate();
}

bool Window::PreRenderCallback()
{
	GUI_TRACE();

	if (m_mouseOffset != Vec2f(zero))
	{
		m_renderManager.GetCamera().MouseCallback(m_mouseOffset);
		m_renderManager.SetRenderDirty();
		m_mouseOffset = zero;
	}

	if (m_cameraMovement != Camera::None)
	{
		m_renderManager.GetCamera().KeyboardCallback(m_cameraMovement);
		m_renderManager.SetRenderDirty();
		m_cameraMovement = Camera::None;
	}

	if (m_renderManager.SetMaxSamples(m_renderGlobals.GetMaxSamples()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetIntegrator(m_renderGlobals.GetIntegrator()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetSampler(m_renderGlobals.GetSampler()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetHideLights(m_renderGlobals.GetHideLights()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetMaxLightSamples(m_renderGlobals.GetMaxLightsSamples()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetMaxBSDFSamples(m_renderGlobals.GetMaxBSDFSamples()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetMaxDepth(m_renderGlobals.GetMaxDepth()))
		m_renderManager.SetRenderDirty();
	if (m_renderManager.SetRussianRouletteDepth(m_renderGlobals.GetRussianRouletteDepth()))
		m_renderManager.SetRenderDirty();

	if (m_renderManager.SetCurrentCamera(m_sceneCamera))
		m_renderManager.SetRenderDirty();

	m_renderManager.SetCurrentBuffer(m_renderGlobals.GetBufferID());
	m_renderManager.SetMaxIterations(m_renderGlobals.GetMaxIterations());

	return true;
}

void Window::SetupGUI()
{
	GUI_TRACE();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	const float currentFrame(glfwGetTime());
	m_deltaTime = currentFrame - m_lastFrame;
	m_lastFrame = currentFrame;

	if (m_profilingState)
		ProfilingWindow();
	if (m_aboutState)
		AboutWindow();
	if (m_renderConfigState)
		RenderConfigWindow();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::BeginMenu("Export to..."))
			{
				if (ImGui::MenuItem("PPM"))
				{
					toPPM(m_renderGlobals.GetWidth(),
							m_renderGlobals.GetHeight(),
							m_renderManager.GetBuffer());
				}

				if (ImGui::MenuItem("EXR"))
				{
					toEXR(m_renderGlobals.GetWidth(),
							m_renderGlobals.GetHeight(),
							m_renderManager.GetBuffer());
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Rendering"))
		{
			if (ImGui::BeginMenu("Integrator"))
			{
				ImGui::RadioButton("Direct", reinterpret_cast<int *>(&m_renderGlobals.m_integratorID), 0);
				ImGui::RadioButton("ForwardPath", reinterpret_cast<int *>(&m_renderGlobals.m_integratorID), 1);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Buffer"))
			{
				ImGui::RadioButton("Beauty", reinterpret_cast<int *>(&m_renderGlobals.m_bufferID), 0);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Sampler"))
			{
				ImGui::RadioButton("Independent", reinterpret_cast<int *>(&m_renderGlobals.m_samplerId), 0);
				ImGui::RadioButton("Uniform", reinterpret_cast<int *>(&m_renderGlobals.m_samplerId), 1);
				ImGui::RadioButton("Stratified", reinterpret_cast<int *>(&m_renderGlobals.m_samplerId), 2);

				ImGui::EndMenu();
			}

			ImGui::Separator();
			ImGui::MenuItem("Config", NULL, &m_renderConfigState);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera"))
		{
			if (ImGui::BeginMenu("Cameras"))
			{
				int i = 0;
				for (const auto& camera : m_renderManager.GetScene()->GetSceneCameras())
					ImGui::RadioButton(camera.c_str(), &m_sceneCamera, i++);

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
					m_renderManager.LoadScene(filepath);
					m_renderManager.SetRenderDirty();
				}
			}
			if (ImGui::MenuItem("Import..."))
			{
				if (const std::string filepath = GetBrowserFilePath(); !filepath.empty())
					if (m_renderManager.ImportScene(filepath))
						m_renderManager.SetRenderDirty();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("Profiling", NULL, &m_profilingState);
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &m_aboutState);

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
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::StopGUI()
{
	GUI_TRACE();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Window::RenderConfigWindow()
{
	GUI_TRACE();
	ImGui::Begin("Render Config", &m_renderConfigState);

	ImGui::InputInt("Width", reinterpret_cast<int*>(&m_renderGlobals.m_width));
	ImGui::InputInt("Height", reinterpret_cast<int*>(&m_renderGlobals.m_height));

	ImGui::Separator();

	ImGui::InputInt("Max Iterations", reinterpret_cast<int*>(&m_renderGlobals.m_maxIterations));
	ImGui::InputInt("Samples", reinterpret_cast<int*>(&m_renderGlobals.m_maxSamplesPerPixel));

	ImGui::Checkbox("Hide Lights", &m_renderGlobals.m_hideLights);

	ImGui::Separator();

	ImGui::InputInt("Max Ligh Samples", reinterpret_cast<int*>(&m_renderGlobals.m_maxLightSamples));
	ImGui::InputInt("Max BSDF Samples", reinterpret_cast<int*>(&m_renderGlobals.m_maxBSDFSamples));

	ImGui::Separator();

	ImGui::InputInt("Max Depth", reinterpret_cast<int*>(&m_renderGlobals.m_maxDepth));
	ImGui::InputInt("Russian Roulette Depth", reinterpret_cast<int*>(&m_renderGlobals.m_russianRouletteDepth));

	ImGui::Separator();


	ImGui::End();
}

void Window::ProfilingWindow()
{
	GUI_TRACE();
	ImGui::Begin("Profiling", &m_profilingState, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Total Samples (iterations * eamples): %d", m_renderManager.GetIterations() * m_renderManager.GetSamples());
	// TODO: Find a good measurement for this.
	// ImGui::Text("Framerate: %.2f FPS / %.2f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)",
			m_renderManager.GetScene()->GetSceneCamera().GetPosition().x,
			m_renderManager.GetScene()->GetSceneCamera().GetPosition().y,
			m_renderManager.GetScene()->GetSceneCamera().GetPosition().z);

	ImGui::End();
}

void Window::AboutWindow()
{
	GUI_TRACE();
	ImGui::Begin("About", &m_aboutState);

	ImGui::Text("Spindulys by Linas Beresna\n\nEmail: linas_beresna@sfu.ca");

	ImGui::End();
}

void Window::KeyboardCallback()
{
	ImGuiIO& guiIO = ImGui::GetIO();
	GUI_TRACE();
	if (guiIO.KeysDown[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(m_window, GL_TRUE);
	}
	if (guiIO.KeysDown[GLFW_KEY_W])
	{
		m_cameraMovement = Camera::Forward;
	}
	if (guiIO.KeysDown[GLFW_KEY_S])
	{
		m_cameraMovement = Camera::Backward;
	}
	if (guiIO.KeysDown[GLFW_KEY_A])
	{
		m_cameraMovement = Camera::Left;
	}
	if (guiIO.KeysDown[GLFW_KEY_D])
	{
		m_cameraMovement = Camera::Right;
	}
	if (guiIO.KeysDown[GLFW_KEY_KP_ADD])
	{
		spdlog::warn("TODO implement key up as amerature something");
	}
	if (guiIO.KeysDown[GLFW_KEY_KP_SUBTRACT])
	{
		spdlog::warn("TODO implement key down as amerature something");
	}
}

void Window::MouseCallback()
{
	GUI_TRACE();

	const Vec2f mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	if (m_firstMouse)
	{
		prevMousePos = Vec2f(mousePos);
		m_firstMouse = false;
	}

	const Vec2f mouseOffset(mousePos.x - prevMousePos.x, mousePos.y - prevMousePos.y);
	prevMousePos = Vec2f(mousePos);

	ImGuiIO& guiIO = ImGui::GetIO();
	if (guiIO.MouseDown[GLFW_MOUSE_BUTTON_RIGHT])
		m_mouseOffset = mouseOffset;
}

void Window::RenderToScreenTexture(int width, int height, const Buffer3f& buffer)
{
	GUI_TRACE();

	// TODO: do something here.
	// if (width != renderGlobals.width || height != renderGlobals.height)
	{
		glBindTexture(GL_TEXTURE_2D, m_screenTextureID);
		glDeleteTextures(1, &m_screenTextureID);

		glGenTextures(1, &m_screenTextureID);
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, m_screenTextureID);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindTexture(GL_TEXTURE_2D, m_screenTextureID);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, buffer.GetPixelData().data());
	m_screenQuadShader.Use();
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_screenTextureID);

	DrawScreenQuad();
}

void Window::SetupScreenQuad(int width, int height)
{
	GUI_TRACE();
	constexpr GLfloat screenQuadVertices[] =
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	// Screen quad geometry.
	glGenVertexArrays(1, &m_screenQuadVAO);
	glGenBuffers(1, &m_screenQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_screenQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

	glBindVertexArray(m_screenQuadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	// Screen quad shader and texture.
	m_screenQuadShader.Setup(VERT_SHADER, FRAG_SHADER);

	glGenTextures(1, &m_screenTextureID);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_screenTextureID);

	// TODO: Investigate why the bellow doesn't work on mac but glTexImage2D does.
	// glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::CleanScreenQuad()
{
	GUI_TRACE();
	glBindVertexArray(m_screenQuadVAO);
	glDeleteVertexArrays(1, &m_screenQuadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_screenQuadVBO);
	glDeleteBuffers(1, &m_screenQuadVBO);

	glBindTexture(GL_TEXTURE_2D, m_screenTextureID);
	glDeleteTextures(1, &m_screenTextureID);

	m_screenQuadVBO = 0;
	m_screenQuadVAO = 0;
	m_screenTextureID = 0;
}

void Window::DrawScreenQuad()
{
	GUI_TRACE();
	glBindVertexArray(m_screenQuadVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

GUI_NAMESPACE_CLOSE_SCOPE
