/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-21 16:20:31
*/

#include "Window.hpp"
#include "Config.hpp" // for Config
#include "Debug.hpp" // for glCheckError
#include "Engine.hpp" // for Stop
#include "Event/Events.hpp" // for Events
#include <SDL.h> // for SDL_Init, SDL_INIT_EVERYTHING
#include <SDL_error.h> // for SDL_GetError
#include <SDL_gamecontroller.h> // for SDL_GameControllerEventState
#include <SDL_joystick.h> // for SDL_JoystickEventState
#include <stdexcept> // for runtime_error

Window* Window::_instance = nullptr;

Window::Window()
{
    Events::Add(this, SDL_WINDOWEVENT);
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

SDL_GLContext Window::context()
{
    return (_get()._gl_context);
}

SDL_Window* Window::sdl_window()
{
    return (_get()._sdl_window);
}

void PrintExtensions()
{
    debugLog("GL Extensions :");
    GLint n;
    glGetIntegerv(GL_NUM_EXTENSIONS, &n);
    for (auto i = 0; i < n; ++i) {
        debugLog(glGetStringi(GL_EXTENSIONS, i));
    }
}

void Window::init(const std::string& name, glm::ivec2 resolution)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    SDL_JoystickEventState(SDL_ENABLE);
    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
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
    debugLog(std::string("GL vendor    : ") + reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    debugLog(std::string("GL renderer  : ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    debugLog(std::string("GL version   : ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    debugLog(std::string("GLSL version : ") + reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    PrintExtensions();
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#ifdef DEBUG_MOD
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, GL_DONT_CARE, nullptr, GL_FALSE);
    glDebugMessageCallback(MessageCallback, 0);
#endif
}

GLbitfield& Window::clear_mask()
{
    return (_get()._clear_mask);
}

glm::ivec2 Window::size()
{
    int w;
    int h;

    SDL_GL_GetDrawableSize(_get()._sdl_window, &w, &h);
    return (glm::vec2(w, h));
}

void Window::resize(const glm::ivec2& size)
{
    SDL_SetWindowSize(_get()._sdl_window, size.x, size.y);
}

void Window::fullscreen(const bool& fullscreen)
{
    SDL_SetWindowFullscreen(_get()._sdl_window,
        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}
