/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:42:21
*/

#include "Window.hpp"
#include "Event/InputDevice.hpp" // for InputDevice

#include "Config.hpp" // for Config
#include "Debug.hpp" // for glCheckError
#include "Engine.hpp" // for Stop
#include "Event/Events.hpp" // for Events
#include <SDL.h> // for SDL_Init, SDL_INIT_EVERYTHING
#include <SDL_error.h> // for SDL_GetError
#include <SDL_gamecontroller.h> // for SDL_GameControllerEventState
#include <SDL_joystick.h> // for SDL_JoystickEventState
#include <stdexcept> // for runtime_error

struct WindowImpl : InputDevice {
    static WindowImpl& _get();
    static WindowImpl* _instance;
    Window::Handle _handle{ nullptr };
    virtual void process_event(SDL_Event*) override;
};

WindowImpl* WindowImpl::_instance = nullptr;

void WindowImpl::process_event(SDL_Event* event)
{
    if (event->window.event == SDL_WINDOWEVENT_CLOSE) {
        Engine::Stop();
    } else if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
        //Window::resize();
    }
}

/*
** window is a singleton
*/

WindowImpl& WindowImpl::_get()
{
    if (_instance == nullptr) {
        _instance = new WindowImpl();
        Events::Add(_instance, SDL_WINDOWEVENT);
    }
    return (*_instance);
}

void Window::Swap()
{
    SDL_GL_SwapWindow((SDL_Window*)WindowImpl::_get()._handle);
}

Window::Handle Window::GetHandle()
{
    return WindowImpl::_get()._handle;
}

void Window::Init(const std::string& name, glm::ivec2 resolution)
{
    if (GetHandle() != nullptr)
        return;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    WindowImpl::_get()._handle = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (GetHandle() == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

glm::ivec2 Window::GetSize()
{
    int w;
    int h;

    SDL_GL_GetDrawableSize((SDL_Window*)GetHandle(), &w, &h);
    return (glm::vec2(w, h));
}

void Window::SetSize(const glm::ivec2& size)
{
    SDL_SetWindowSize((SDL_Window*)GetHandle(), size.x, size.y);
}

void Window::SetFullscreen(const bool& fullscreen)
{
    SDL_SetWindowFullscreen((SDL_Window*)GetHandle(),
        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}
