#pragma once

#include "Window.hpp"
#include "Renderer/Renderer.hpp"

/**
 * @brief Immediat UI interface
*/
namespace UIManager {
	void Init(Window::Handle windowHandle, Renderer::Context &renderContext);
	void Begin(const char * = nullptr);
	void End();
	void Render();
};