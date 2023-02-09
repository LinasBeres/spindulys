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

		void RenderWindow(const std::string& scenePath);

		// Callbacks for Renderer
		bool CloseWindow() const { return glfwWindowShouldClose(m_window); }
		bool PreRenderCallback();
		void UpdateScreen(const Buffer3f& buffer) { m_currentBuffer = buffer; m_updateScreen = true; glfwPostEmptyEvent(); }

	private:
		bool m_firstMouse = true;
		bool m_renderConfigState = false;
		bool m_profilingState = true;
		bool m_aboutState = false;
		bool m_updateScreen = false;

		float m_deltaTime = 0.0f;
		float m_lastFrame = 0.0f;

		GLFWwindow* m_window;

		bool m_updateRenderer = false;
		int m_sceneCamera = 0;
		RenderGlobals m_renderGlobals;

		Camera::CAMERA_MOVEMENTS m_cameraMovement = Camera::None;

		Vec2f prevMousePos = Vec2f(m_renderGlobals.GetWidth() / 2.0f, m_renderGlobals.GetHeight() / 2.0f);
		Vec2f m_mouseOffset = Vec2f(zero);

		GLuint m_screenQuadVAO;
		GLuint m_screenQuadVBO;
		GLuint m_screenTextureID;

		GLShader m_screenQuadShader;

		CPURenderManager m_renderManager;

		Buffer3f m_currentBuffer = Buffer3f(m_renderGlobals.GetWidth(), m_renderGlobals.GetHeight());

	private:
		bool InitGLFW();
		bool InitWindow();

		// GL methods
		void RenderToScreenTexture(int width, int height, const Buffer3f& buffer);
		void SetupScreenQuad(int width, int height);
		void CleanScreenQuad();
		void DrawScreenQuad();

		// Gui methods
		void SetupGUI();
		void RenderGUI();
		void StopGUI();
		void RenderConfigWindow();
		void ProfilingWindow();
		void AboutWindow();

		// Callbacks for GLFW
		void KeyboardCallback();
		void MouseCallback();

		std::string GetBrowserFilePath() const;
};

GUI_NAMESPACE_CLOSE_SCOPE

#endif // WINDOW_H
