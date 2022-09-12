#include "window.h"

#include <functional>

#include "output_helper.h"


GUI_NAMESPACE_OPEN_SCOPE


Window::Window()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
}

int Window::RenderWindow(const std::string& scenePath)
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(renderGlobals.width, renderGlobals.height, "Spindulys", nullptr, nullptr);
	if (!window)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	gladLoadGL();


	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	CPURenderManager renderManager;

	renderManager.LoadScene(!scenePath.empty() ? scenePath : TEST_SCENE);
	renderManager.GetCamera().SetResolution(Vec2f(renderGlobals.width, renderGlobals.height));

	RenderManager::StopRenderer stopRenderingFunction = std::bind(&Window::CloseWindow, this);
	renderManager.SetStopRendererCallback(stopRenderingFunction);


	RenderManager::RegisterUpdates updateRendererFunction =
		std::bind(&Window::PreRenderCallback, this, std::placeholders::_1);
	renderManager.SetUpdateCallback(updateRendererFunction);

	RenderManager::DrawBuffer drawBufferFunction =
		std::bind(&Window::RenderToScreenTexture, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	renderManager.SetBufferCallback(drawBufferFunction);

	SetupScreenQuad(renderGlobals.width, renderGlobals.height);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	renderManager.Render();

	StopGUI();
	glfwTerminate();

	return 0;
}

bool Window::PreRenderCallback(RenderManager* renderManager)
{
	float currentFrame(glfwGetTime());
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	ImGuiIO& guiIO = ImGui::GetIO();
	(void)guiIO;

	SetupGUI(renderManager);
	KeyboardCallback(guiIO, renderManager);

	Vec2f mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
	if (prevMousePos.x != mousePos.x || prevMousePos.y != mousePos.y)
		MouseCallback(guiIO, mousePos, renderManager);

	if(renderManager->SetSamples(renderGlobals.samples))
		renderManager->SetRenderDirty();
	if(renderManager->SetIntegrator(renderGlobals.integratorID))
		renderManager->SetRenderDirty();
	if (renderManager->SetCurrentCamera(sceneCamera))
		renderManager->SetRenderDirty();

	renderManager->SetCurrentBuffer(renderGlobals.bufferID);
	renderManager->SetMaxIterations(renderGlobals.maxIterations);

	return true;
}

void Window::SetupGUI(RenderManager* renderManager)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
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
			if (ImGui::BeginMenu("Render to..."))
			{
				if (ImGui::MenuItem("PPM"))
				{
					renderManager->Trace(1);

					toPPM(renderGlobals.width,
							renderGlobals.height,
							renderManager->GetBuffer());
				}

				if (ImGui::MenuItem("EXR"))
				{
					renderManager->Trace(1);

					toEXR(renderGlobals.width,
							renderGlobals.height,
							renderManager->GetBuffer());
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Export to..."))
			{
				if (ImGui::MenuItem("PPM"))
				{
					toPPM(renderGlobals.width,
							renderGlobals.height,
							renderManager->GetBuffer());
				}

				if (ImGui::MenuItem("EXR"))
				{
					toEXR(renderGlobals.width,
							renderGlobals.height,
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
				ImGui::RadioButton("UDPT", reinterpret_cast<int *>(&renderGlobals.integratorID), 0);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Buffer"))
			{
				ImGui::RadioButton("Beauty", reinterpret_cast<int *>(&renderGlobals.bufferID), 0);
				ImGui::RadioButton("Diffuse", reinterpret_cast<int *>(&renderGlobals.bufferID), 1);
				ImGui::RadioButton("Position", reinterpret_cast<int *>(&renderGlobals.bufferID), 2);
				ImGui::RadioButton("Normal", reinterpret_cast<int *>(&renderGlobals.bufferID), 3);
				ImGui::RadioButton("Debug", reinterpret_cast<int *>(&renderGlobals.bufferID), 4);

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
			if (ImGui::BeginMenu("Load..."))
			{
				if (ImGui::MenuItem("Cup and Saucer"))
				{
					renderManager->LoadScene(CUPS_SCENE);
					renderManager->SetRenderDirty();
				}

				if (ImGui::MenuItem("Stormtroopers"))
				{
					renderManager->LoadScene(STORMTROOPERS_SCENE);
					renderManager->SetRenderDirty();
				}

				ImGui::EndMenu();
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

void Window::RenderGUI()
{
	ImGui::Render();
}

void Window::StopGUI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Window::RenderConfigWindow(bool& guiOpen)
{
	ImGui::Begin("Render Config", &guiOpen);

	ImGui::InputInt("Width", &renderGlobals.width);
	ImGui::InputInt("Height", &renderGlobals.height);
	ImGui::InputInt("Max Iterations", &renderGlobals.maxIterations);
	ImGui::InputInt("Samples", &renderGlobals.samples);
	ImGui::InputInt("Depth", &renderGlobals.depth);

	ImGui::Separator();


	ImGui::End();
}

void Window::ProfilingWindow(bool& guiOpen, RenderManager* renderManager)
{
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
	ImGui::Begin("About", &guiOpen);

	ImGui::Text("Spindulys by Linas Beresna\n\nEmail: linas_beresna@sfu.ca");

	ImGui::End();
}

void Window::KeyboardCallback(ImGuiIO& guiIO, RenderManager* renderManager)
{
	if (guiIO.KeysDown[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (guiIO.KeysDown[GLFW_KEY_W])
	{
		renderManager->GetCamera().KeyboardCallback(Camera::Forward, deltaTime);
		renderManager->SetRenderDirty();
	}
	if (guiIO.KeysDown[GLFW_KEY_S])
	{
		renderManager->GetCamera().KeyboardCallback(Camera::Backward, deltaTime);
		renderManager->SetRenderDirty();
	}
	if (guiIO.KeysDown[GLFW_KEY_A])
	{
		renderManager->GetCamera().KeyboardCallback(Camera::Left, deltaTime);
		renderManager->SetRenderDirty();
	}
	if (guiIO.KeysDown[GLFW_KEY_D])
	{
		renderManager->GetCamera().KeyboardCallback(Camera::Right, deltaTime);
		renderManager->SetRenderDirty();
	}

	if (guiIO.KeysDown[GLFW_KEY_KP_ADD])
	{
		// renderManager->mainCamera->SetAperatureRadius(renderManager->mainCamera->GetAperatureRadius() + 0.005f);
		renderManager->SetRenderDirty();
	}
	if (guiIO.KeysDown[GLFW_KEY_KP_SUBTRACT])
	{
		// renderManager->mainCamera->SetAperatureRadius(renderManager->mainCamera->GetAperatureRadius() - 0.005f);
		renderManager->SetRenderDirty();
	}
}

void Window::MouseCallback(ImGuiIO& guiIO, Vec2f mousePos,RenderManager* renderManager)
{
	if (firstMouse)
	{
		prevMousePos = Vec2f(mousePos);

		firstMouse = false;
	}

	const Vec2f mouseOffset(mousePos.x - prevMousePos.x, mousePos.y - prevMousePos.y);
	prevMousePos = Vec2f(mousePos);

	if (guiIO.MouseDown[GLFW_MOUSE_BUTTON_RIGHT])
	{
		if (mouseOffset != Vec2f())
		{
			renderManager->GetCamera().MouseCallback(mouseOffset);
			renderManager->SetRenderDirty();
		}
	}
}

void Window::RenderToScreenTexture(int width, int height, const Buffer3f& buffer)
{
	glBindTexture(GL_TEXTURE_2D, screenTextureID);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, buffer.GetPixelData().data());
	screenQuadShader.Use();
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, screenTextureID);

	DrawScreenQuad();
	RenderGUI();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(window);
}

void Window::SetupScreenQuad(int width, int height)
{
	constexpr GLfloat screenQuadVertices[] =
	{
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	// Screen quad geometry.
	glGenVertexArrays(1, &screenQuadVAO);
	glGenBuffers(1, &screenQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenQuadVertices), screenQuadVertices, GL_STATIC_DRAW);

	glBindVertexArray(screenQuadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);

	// Screen quad shader and texture.
	screenQuadShader.Setup(VERT_SHADER, FRAG_SHADER);

	glGenTextures(1, &screenTextureID);
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, screenTextureID);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::CleanScreenQuad()
{
	glBindBuffer(GL_ARRAY_BUFFER, screenQuadVBO);
	glDeleteBuffers(1, &screenQuadVBO);

	screenQuadVBO = 0;
}

void Window::DrawScreenQuad()
{
	glBindVertexArray(screenQuadVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

GUI_NAMESPACE_CLOSE_SCOPE
