#include "window.h"

#include "output_helper.h"


GUI_NAMESPACE_OPEN_SCOPE

int Window::RenderWindow()
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

	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGL();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& guiIO = ImGui::GetIO(); (void)guiIO;

	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	renderManager.LoadScene("/home/lba42/Documents/testRenderers/spindulys/res/scenes/cupandsaucer.usdz");
	renderManager.mainCamera->SetResolution(Vec2f(renderGlobals.width, renderGlobals.height));
	renderManager.mainCamera->Init();


	SetupScreenQuad(renderGlobals.width, renderGlobals.height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();

		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//--------------
		// GUI setting & callbacks
		//--------------
		SetupGUI();

		KeyboardCallback(guiIO);

		// ImVec2 mousePos(ImGui::GetMousePos());
		Vec2f mousePos(ImGui::GetMousePos().x, ImGui::GetMousePos().y);

		if (prevMousePos.x != mousePos.x || prevMousePos.y != mousePos.y)
			MouseCallback(guiIO, mousePos);

		// TODO: Will make use of Qt's callback system once the GUI will be revamped.
		if (renderManager.renderGlobals.integratorID != renderGlobals.integratorID)
		{
			renderManager.renderGlobals.integratorID = renderGlobals.integratorID;

			renderReset = true;
		}

		//--------------
		// CPU Rendering
		//--------------
		if (!pauseState)
		{
			// If anything changed in the camera, scene, settings, etc, we flush the rendered data and reinit them again
			if (renderReset)
				ResetRenderer();

			// Progressive rendering
			if (iterations++ < 20)
			{
				renderManager.Trace(iterations);
				// std::cerr << renderManager.GetBuffer() << "\n";
			}

			RenderToScreenTexture(renderGlobals.width, renderGlobals.height, renderManager.GetBuffer());
		}

		DrawScreenQuad();

		//----------------
		// GUI rendering
		//----------------
		RenderGUI();

		// ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// glfwGetFramebufferSize(window, &renderGlobals.width, &renderGlobals.height);
		// glViewport(0, 0, renderGlobals.width, renderGlobals.height);
		// glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}

	//---------
	// Cleaning
	//---------
	StopGUI();

	glfwTerminate();

	return 0;
}

void Window::ResetRenderer()
{
	frontBuffer.Clean(renderGlobals.width, renderGlobals.height);

	iterations = 0;
	renderReset = false;
}

void Window::SetupGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (profilingState)
		ProfilingWindow(profilingState);
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
					renderManager.Trace(1);

					toPPM(renderGlobals.width,
							renderGlobals.height,
							renderManager.GetBuffer());
				}

				if (ImGui::MenuItem("EXR"))
				{
					renderManager.Trace(1);

					toEXR(renderGlobals.width,
							renderGlobals.height,
							renderManager.GetBuffer());
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Export to..."))
			{
				if (ImGui::MenuItem("PPM"))
				{
					toPPM(renderGlobals.width,
							renderGlobals.height,
							renderManager.GetBuffer());
				}

				if (ImGui::MenuItem("EXR"))
				{
					toEXR(renderGlobals.width,
							renderGlobals.height,
							renderManager.GetBuffer());
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
				ImGui::RadioButton("Diffuse", reinterpret_cast<int *>(&renderGlobals.integratorID), 1);
				ImGui::RadioButton("Occlusion", reinterpret_cast<int *>(&renderGlobals.integratorID), 2);
				ImGui::RadioButton("Position", reinterpret_cast<int *>(&renderGlobals.integratorID), 3);
				ImGui::RadioButton("Normal", reinterpret_cast<int *>(&renderGlobals.integratorID), 4);
				ImGui::RadioButton("Debug", reinterpret_cast<int *>(&renderGlobals.integratorID), 5);

				ImGui::EndMenu();
			}

			ImGui::Separator();
			ImGui::MenuItem("Config", NULL, &renderConfigState);

			if (!swapState)
			{
				ImGui::Separator();
				ImGui::Checkbox("Pause Render", &pauseState);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Camera"))
		{
			ImGui::Checkbox("Jitter Rays", &renderGlobals.rayJitter);

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::BeginMenu("Load..."))
			{
				if (ImGui::MenuItem("Cup and Saucer"))
				{
					renderManager.LoadScene("/home/lba42/Documents/testRenderers/spindulys/res/scenes/cupandsaucer.usdz");

					renderReset = true;
				}

				if (ImGui::MenuItem("Stormtroopers"))
				{
					renderManager.LoadScene("/home/lba42/Documents/testRenderers/spindulys/res/scenes/stormtroopers.usdc");

					renderReset = true;
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
	ImGui::InputInt("Samples", &renderGlobals.samples);
	ImGui::InputInt("Depth", &renderGlobals.depth);

	ImGui::Separator();

	if (!swapState)
	{
		if (ImGui::Button("Save To Back Buffer"))
		{
			backBuffer.Clean(renderGlobals.width, renderGlobals.height);
			backBuffer.Swap(frontBuffer);
		}
	}
	if (ImGui::Button("Swap Buffers"))
	{
		frontBuffer.Swap(backBuffer);
		RenderToScreenTexture(renderGlobals.width, renderGlobals.height, frontBuffer);

		swapState = !swapState;
		pauseState = true;
	}

	ImGui::End();
}

void Window::ProfilingWindow(bool& guiOpen)
{
	ImGui::Begin("Profiling", &guiOpen, ImGuiWindowFlags_NoTitleBar |ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Framerate: %.2f FPS / %.2f ms", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", renderManager.mainCamera->GetPosition().x, renderManager.mainCamera->GetPosition().y, renderManager.mainCamera->GetPosition().z);

	ImGui::End();
}

void Window::AboutWindow(bool& guiOpen)
{
	ImGui::Begin("About", &guiOpen);

	ImGui::Text("Spindulys by Linas Beresna\n\nEmail: linas_beresna@sfu.ca");

	ImGui::End();
}

void Window::KeyboardCallback(ImGuiIO& guiIO)
{
	if (guiIO.KeysDown[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (guiIO.KeysDown[GLFW_KEY_W])
	{
		renderManager.mainCamera->KeyboardCallback(Camera::Forward, deltaTime);

		renderReset = true;
	}
	if (guiIO.KeysDown[GLFW_KEY_S])
	{
		renderManager.mainCamera->KeyboardCallback(Camera::Backward, deltaTime);

		renderReset = true;
	}
	if (guiIO.KeysDown[GLFW_KEY_A])
	{
		renderManager.mainCamera->KeyboardCallback(Camera::Left, deltaTime);
		renderReset = true;
	}
	if (guiIO.KeysDown[GLFW_KEY_D])
	{
		renderManager.mainCamera->KeyboardCallback(Camera::Right, deltaTime);

		renderReset = true;
	}

	if (guiIO.KeysDown[GLFW_KEY_KP_ADD])
	{
		if (guiIO.KeysDown[GLFW_KEY_LEFT_CONTROL])
			renderManager.mainCamera->SetFocalDistance(renderManager.mainCamera->GetFocalDistance() + 0.1f);
		else
			renderManager.mainCamera->SetAperatureRadius(renderManager.mainCamera->GetAperatureRadius() + 0.005f);

		renderReset = true;
	}
	if (guiIO.KeysDown[GLFW_KEY_KP_SUBTRACT])
	{
		if (guiIO.KeysDown[GLFW_KEY_LEFT_CONTROL])
			renderManager.mainCamera->SetFocalDistance(renderManager.mainCamera->GetFocalDistance() - 0.1f);
		else
			renderManager.mainCamera->SetAperatureRadius(renderManager.mainCamera->GetAperatureRadius() - 0.005f);

		renderReset = true;
	}
}

void Window::MouseCallback(ImGuiIO& guiIO, Vec2f mousePos)
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
			renderManager.mainCamera->MouseCallback(mouseOffset);

			renderReset = true;
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
	screenQuadShader.Setup("/home/lba42/Documents/testRenderers/spindulys/res/shaders/screenQuad.vert",
			"/home/lba42/Documents/testRenderers/spindulys/res/shaders/screenQuad.frag");

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
