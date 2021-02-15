#include "UI/UIManager.hpp"
#include "Window.hpp"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <SDL.h>

void UIManager::Init(Window::Handle windowHandle, Render::Context renderContext) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL((SDL_Window*)windowHandle, renderContext);
	ImGui_ImplOpenGL3_Init();
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
}

void UIManager::Begin(const char* name)
{
	ImGui::Begin(name);
}

void UIManager::End()
{
	ImGui::End();
}

void UIManager::Render()
{
	ImGui::Render();
}
