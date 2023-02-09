#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <spindulys/math/vec2.h>
#include <spindulys/buffer.h>

#include <render/renderManager.h>
#include <render/cpuRenderManager.h>

#include "spindulysGUI.h"

#include "opengl/glshader.h"

GUI_NAMESPACE_OPEN_SCOPE

class Window
{
	public:
		Window();
		~Window() = default;

		int RenderWindow(const std::string& scenePath);
		void SetupGUI();
		void RenderGUI();
		void StopGUI();
		void RenderConfigWindow(bool &guiOpen);
		void ProfilingWindow(bool& guiOpen);
		void AboutWindow(bool &guiOpen);
		void KeyboardCallback(ImGuiIO &guiIO);
		void MouseCallback(ImGuiIO &guiIO, Vec2f mousePos);

		void RenderToScreenTexture(int width, int height, const Buffer3f& buffer);
		void SetupScreenQuad(int width, int height);
		void CleanScreenQuad();
		void DrawScreenQuad();

		bool CloseWindow() { return glfwWindowShouldClose(window); }
		bool PreRenderCallback();

		void UpdateScreen(void) { updateScreen = true; glfwPostEmptyEvent(); }

	private:
		bool firstMouse = true;
		bool renderConfigState = false;
		bool profilingState = true;
		bool aboutState = false;
		bool updateScreen = false;
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		GLFWwindow* window;
		int sceneCamera = 0;
		RenderGlobals renderGlobals;

		Vec2f prevMousePos = Vec2f(renderGlobals.GetWidth() / 2.0f, renderGlobals.GetHeight() / 2.0f);

		GLuint screenQuadVAO;
		GLuint screenQuadVBO;
		GLuint screenTextureID;

		GLShader screenQuadShader;

		CPURenderManager renderManager;

	private:
		std::string GetBrowserFilePath() const;
};

GUI_NAMESPACE_CLOSE_SCOPE

#endif // WINDOW_H
