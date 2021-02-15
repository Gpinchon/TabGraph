#pragma once

#include "Window.hpp"
#include "Render.hpp"

/**
 * @brief Immediat UI interface
*/
namespace UIManager {
	void Init(Window::Handle windowHandle, Render::Context renderContext);
	void Begin(const char * = nullptr);
	void End();
	void Render();
};