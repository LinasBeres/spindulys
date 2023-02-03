#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>


#include <glad/glad.h>
#include <SDL.h>

// Dear ImGui
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>

#include <spindulys/math/vec2.h>
#include <spindulys/buffer.h>

#include <render/renderManager.h>
#include <render/cpuRenderManager.h>

#include "spindulysGUI.h"

GUI_NAMESPACE_OPEN_SCOPE

class Window
{
	public:
		Window();
		~Window() = default;

		int RenderWindow(const std::string& scenePath);
		void SetupGUI(RenderManager* renderManager);
		void RenderGUI();
		void StopGUI();
		void RenderConfigWindow(bool &guiOpen);
		void ProfilingWindow(bool& guiOpen, RenderManager* renderManager);
		void AboutWindow(bool &guiOpen);
		void KeyboardCallback(ImGuiIO &guiIO, RenderManager* renderManager);
		void MouseCallback(ImGuiIO &guiIO, Vec2f mousePos, RenderManager* renderManager);

		void RenderToScreenTexture(int width, int height, const Buffer3f& buffer);
		void SetupScreenQuad(int width, int height);
		void CleanScreenQuad();
		void DrawScreenQuad();

		bool CloseWindow() { return false; }
		bool PreRenderCallback(RenderManager*);

	private:
		bool firstMouse = true;
		bool renderConfigState = false;
		bool profilingState = true;
		bool aboutState = false;
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		int sceneCamera = 0;
		RenderGlobals renderGlobals;

		Vec2f prevMousePos = Vec2f(renderGlobals.GetWidth() / 2.0f, renderGlobals.GetHeight() / 2.0f);

	private:
		std::string GetBrowserFilePath() const;
};

GUI_NAMESPACE_CLOSE_SCOPE

#endif // WINDOW_H
