/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:25:09
*/

#include "Window.hpp"
#include <SDL2/SDL.h>                 // for SDL_Init, SDL_INIT_EVERYTHING
#include <SDL2/SDL_error.h>           // for SDL_GetError
#include <SDL2/SDL_gamecontroller.h>  // for SDL_GameControllerEventState
#include <SDL2/SDL_joystick.h>        // for SDL_JoystickEventState
#include <stdexcept>                  // for runtime_error
#include "Config.hpp"                 // for Config
#include "Debug.hpp"                  // for glCheckError
#include "Engine.hpp"                 // for Stop
#include "Events.hpp"                 // for Events

Window* Window::_instance = nullptr;

Window::Window()
{
    Events::add(this, SDL_WINDOWEVENT);
}

void Window::process_event(SDL_Event* event)
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

Window& Window::_get()
{
    if (_instance == nullptr)
        _instance = new Window();
    return (*_instance);
}

void Window::swap()
{
    SDL_GL_SwapWindow(_get()._sdl_window);
}

SDL_GLContext   Window::context()
{
    return (_get()._gl_context);
}

SDL_Window      *Window::sdl_window()
{
    return (_get()._sdl_window);
}

void Window::init(const std::string& name, VEC2 resolution)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);
    //SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, Config::Msaa());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    if (nullptr == _get()._sdl_window)
        _get()._sdl_window = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y,
            SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (_get()._sdl_window == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    _get()._gl_context = SDL_GL_CreateContext(_get()._sdl_window);
    if (_get()._gl_context == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
    _get()._clear_mask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    glewExperimental = GL_TRUE;
    auto error = glewInit();
    if (error != GLEW_OK) {
        throw std::runtime_error(reinterpret_cast<const char*>(glewGetErrorString(error)));
    }
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glCheckError();
}

GLbitfield& Window::clear_mask()
{
    return (_get()._clear_mask);
}

VEC2 Window::size()
{
    int w;
    int h;

    SDL_GL_GetDrawableSize(_get()._sdl_window, &w, &h);
    return (new_vec2(w, h));
}

void Window::resize(const VEC2& size)
{
    SDL_SetWindowSize(_get()._sdl_window, size.x, size.y);
}

void Window::fullscreen(const bool& fullscreen)
{
    SDL_SetWindowFullscreen(_get()._sdl_window,
        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}
