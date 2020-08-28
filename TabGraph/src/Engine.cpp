/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 13:18:12
*/

#include "Engine.hpp"
#include "Config.hpp" // for Config
#include "Environment.hpp" // for Environment
#include "Input/Events.hpp" // for Events
#include "Node.hpp" // for Node
#include "Parser/GLSL.hpp" // for GLSL, PostShader
#include "Render.hpp" // for AddPostTreatment, RequestRedraw
#include "Scene/Scene.hpp"
#include "Texture/Cubemap.hpp" // for Cubemap
#include "Texture/TextureParser.hpp" // for TextureParser
#include "Window.hpp" // for Window

#include <SDL_events.h> // for SDL_PumpEvents, SDL_SetEventFilter
#include <SDL_filesystem.h> // for SDL_GetBasePath
#include <SDL_timer.h> // for SDL_GetTicks
#include <SDL_video.h> // for SDL_GL_MakeCurrent
#include <filesystem>
#include <atomic> // for atomic
#include <chrono> // for milliseconds
#include <iostream> // for operator<<, endl, basic_ostream
#include <memory> // for shared_ptr, __shared_ptr_access
#include <mutex> // for mutex
#include <thread> // for sleep_for
#include <filesystem>

#ifdef _WIN32
#include <io.h> // for getcwd
#include <direct.h>
#else
#include <sys/io.h> // for getcwd
#endif

#ifndef _getcwd
#define _getcwd getcwd
#endif //_getcwd

struct EnginePrivate {
    EnginePrivate();
    static EnginePrivate& Get();
    static void LoadRes(void);
    static void Update(void);
    static void FixedUpdate(void);
    std::atomic<bool> loop { false };
    int8_t swapInterval { 1 };
    double deltaTime { 0 };
    double fixedDeltaTime { 0 };
    std::filesystem::path programPath;
    std::filesystem::path execPath;
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
    static EnginePrivate* _instance = nullptr;
    if (_instance == nullptr)
        _instance = new EnginePrivate();
    return (*_instance);
}

void EnginePrivate::LoadRes()
{
    struct dirent* e;
    std::filesystem::path folder;

    folder = Engine::ProgramPath() / "res/hdr/";
    if (std::filesystem::exists(folder)) {
        for (const auto& entry : std::filesystem::directory_iterator(folder)) {
            if (entry.path().string()[0] == '.')
                continue;
            auto name = entry.path().filename().string();
            auto newEnv = Environment::Create(name);
            newEnv->set_diffuse(Cubemap::Create(name + "Cube", TextureParser::parse(name, (entry.path() / "environment.hdr").string())));
            newEnv->set_irradiance(Cubemap::Create(name + "CubeDiffuse", TextureParser::parse(name + "Diffuse", (entry.path() / "diffuse.hdr").string())));
        }
    }
    folder = Engine::ProgramPath() / "res/skybox/";
    if (std::filesystem::exists(folder)) {
        for (const auto& entry : std::filesystem::directory_iterator(folder)) {
            if (entry.path().string()[0] == '.')
                continue;
            std::string name = entry.path().string();
            auto newEnv = Environment::Create(name);
            try {
                newEnv->set_diffuse(Cubemap::parse(folder));
            }
            catch (std::exception& e) {
                std::cout << e.what() << std::endl;
                continue;
            }
            try {
                newEnv->set_irradiance(Cubemap::parse(folder / "light"));
            }
            catch (std::exception& e) {
                std::cout << e.what() << std::endl;
            }
        }
    }
    Environment::set_current(Environment::Get(0));
}

void Engine::Init()
{
    Window::init(Config::Get("WindowName", std::string("")), Config::Get("WindowSize", glm::vec2(1280, 720)));
    static auto SSAOShaderCode =
#include "ssao.frag"
        ;
    static auto SSAOShader = Shader::Create("SSAO", PostShader);
    SSAOShader->Stage(GL_FRAGMENT_SHADER)->SetTechnique(SSAOShaderCode);
    Render::AddPostTreatment(SSAOShader);
    EnginePrivate::Get().LoadRes();
    Engine::SetSwapInterval(Config::Get("SwapInterval", -1));
}

int event_filter(void* arg, SDL_Event* event)
{
    return (Events::filter(arg, event));
}

void Engine::Start()
{
    double ticks;
    double lastTicks;
    double fixedTiming = lastTicks = SDL_GetTicks() / 1000.f;

    SDL_SetEventFilter(event_filter, nullptr);
    SDL_GL_MakeCurrent(Window::sdl_window(), nullptr);
    Render::Start();
    while (EnginePrivate::Get().loop) {
        //std::this_thread::sleep_for(std::chrono::milliseconds(16));
        if (Render::NeedsUpdate())
            continue;
        ticks = SDL_GetTicks() / 1000.0;
        EnginePrivate::Get().deltaTime = ticks - lastTicks;
        lastTicks = ticks;
        SDL_PumpEvents();
        Scene::Current()->UpdateCPU(EnginePrivate::Get().deltaTime);
        if (ticks - fixedTiming >= 0.015) {
            EnginePrivate::Get().fixedDeltaTime = ticks - fixedTiming;
            fixedTiming = ticks;
            Events::refresh();
            Scene::Current()->FixedUpdateCPU(EnginePrivate::Get().fixedDeltaTime);
        }
        Render::RequestRedraw();
    }
    Render::Stop();
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

double Engine::DeltaTime()
{
    return (EnginePrivate::Get().deltaTime);
}

double Engine::FixedDeltaTime(void)
{
    return EnginePrivate::Get().fixedDeltaTime;
}

const std::filesystem::path Engine::ProgramPath()
{
    return (EnginePrivate::Get().programPath);
}

const std::filesystem::path Engine::ExecutionPath()
{
    return (EnginePrivate::Get().execPath);
}

const std::filesystem::path Engine::ResourcePath()
{
    return (Engine::ProgramPath() / "res");
}
