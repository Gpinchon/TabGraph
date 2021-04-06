/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:41:46
*/

#include "Engine.hpp"
#include "Config.hpp" // for Config
#include "Skybox.hpp" // for Skybox
#include "Event/Events.hpp" // for Events
#include "Node.hpp" // for Node
#include "Renderer/Renderer.hpp" // for AddPostTreatment, RequestRedraw
#include "Scene/Scene.hpp"
#include "Window.hpp" // for Window

#include <SDL_events.h> // for SDL_PumpEvents, SDL_SetEventFilter
#include <SDL_filesystem.h> // for SDL_GetBasePath
#include <SDL_timer.h> // for SDL_GetTicks
#include <SDL_video.h> // for SDL_GL_MakeCurrent
#include <atomic> // for atomic
#include <chrono> // for milliseconds
#include <filesystem>
#include <iostream> // for operator<<, endl, basic_ostream
#include <memory> // for shared_ptr, __shared_ptr_access
#include <mutex> // for mutex
#include <thread> // for sleep_for

#ifdef _WIN32
#include <direct.h>
#include <io.h> // for getcwd
#else
#include <sys/io.h> // for getcwd
#endif

#ifndef _getcwd
#define _getcwd getcwd
#endif //_getcwd

struct EnginePrivate {
    EnginePrivate();
    static EnginePrivate& Get();
    std::atomic<bool> loop { false };
    int8_t swapInterval { 1 };
    std::filesystem::path programPath;
    std::filesystem::path execPath;
    Signal<float> onFixedUpdate;
    Signal<float> onUpdate;
};

EnginePrivate::EnginePrivate()
{
    loop = true;
    swapInterval = 1;
    execPath = std::filesystem::current_path();
    programPath = std::filesystem::absolute(SDL_GetBasePath()).parent_path();
}

EnginePrivate& EnginePrivate::Get()
{
    static EnginePrivate _instance;
    return _instance;
}

void Engine::Init()
{
    Window::Init(Config::Global().Get("WindowName", std::string("")), Config::Global().Get("WindowSize", glm::vec2(1280, 720)));
    Renderer::Init();
    Engine::SetSwapInterval(Config::Global().Get("SwapInterval", 0));
}

void Engine::Start()
{
    double ticks;
    double lastTicks;
    double fixedTiming = lastTicks = SDL_GetTicks() / 1000.f;

    //SDL_GL_MakeCurrent((SDL_Window*)Window::GetHandle(), Renderer::GetContext());
    SDL_GL_SetSwapInterval(Engine::SwapInterval());
    while (EnginePrivate::Get().loop) {
        ticks = SDL_GetTicks() / 1000.0;
        SDL_PumpEvents();
        EnginePrivate::Get().onUpdate(ticks - lastTicks);
        if (ticks - fixedTiming >= 0.015) {
            EnginePrivate::Get().onFixedUpdate(ticks - fixedTiming);
            fixedTiming = ticks;
        }
        Renderer::RenderFrame();
        lastTicks = ticks;
    }
}

void Engine::Stop(void)
{
    EnginePrivate::Get().loop = false;
}

void Engine::SetSwapInterval(int8_t i)
{
    EnginePrivate::Get().swapInterval = i;
}

int8_t Engine::SwapInterval()
{
    return (EnginePrivate::Get().swapInterval);
}

const std::filesystem::path Engine::ProgramPath()
{
    return EnginePrivate::Get().programPath;
}

const std::filesystem::path Engine::ExecutionPath()
{
    return EnginePrivate::Get().execPath;
}

const std::filesystem::path Engine::ResourcePath()
{
    return (Engine::ProgramPath() / "res");
}

Signal<float>& Engine::OnFixedUpdate()
{
    return EnginePrivate::Get().onFixedUpdate;
}

Signal<float>& Engine::OnUpdate()
{
    return EnginePrivate::Get().onUpdate;
}
