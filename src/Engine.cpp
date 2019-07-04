/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 17:50:56
*/

#include "Engine.hpp"
#include "Config.hpp" // for Config
#include "Cubemap.hpp" // for Cubemap
#include "Environment.hpp" // for Environment
#include "Events.hpp" // for Events
#include "Node.hpp" // for Node
#include "Render.hpp" // for AddPostTreatment, RequestRedraw
#include "TextureParser.hpp" // for TextureParser
#include "Window.hpp" // for Window
#include "parser/GLSL.hpp" // for GLSL, PostShader
#include "parser/InternalTools.hpp" // for convert_backslash
#include <SDL2/SDL_events.h> // for SDL_PumpEvents, SDL_SetEventFilter
#include <SDL2/SDL_filesystem.h> // for SDL_GetBasePath
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <SDL2/SDL_video.h> // for SDL_GL_MakeCurrent
#include <atomic> // for atomic
#include <bits/exception.h> // for exception
#include <chrono> // for milliseconds
#include <dirent.h> // for opendir, readdir, dirent, closedir
#include <io.h> // for getcwd
#include <iostream> // for operator<<, endl, basic_ostream
#include <memory> // for shared_ptr, __shared_ptr_access
#include <mutex> // for mutex
#include <thread> // for sleep_for

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
    std::string programPath { "" };
    std::string execPath { "" };
    std::mutex updateMutex;
};

EnginePrivate::EnginePrivate()
{
    loop = true;
    swapInterval = 1;
    execPath = convert_backslash(_getcwd(nullptr, 4096)) + "/";
    programPath = convert_backslash(SDL_GetBasePath());
    programPath = programPath.substr(0, programPath.find_last_of('/'));
    programPath += "/";
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
    DIR* dir;
    struct dirent* e;
    std::string folder;

    folder = Engine::ProgramPath() + "res/hdr/";
    dir = opendir(folder.c_str());
    while (dir != nullptr && (e = readdir(dir)) != nullptr) {
        if (e->d_name[0] == '.') {
            continue;
        }
        std::string name = e->d_name;
        auto newEnv = Environment::create(name);
        newEnv->set_diffuse(Cubemap::create(name + "Cube", TextureParser::parse(name, folder + name + "/environment.hdr")));
        newEnv->set_irradiance(Cubemap::create(name + "CubeDiffuse", TextureParser::parse(name + "Diffuse", folder + name + "/diffuse.hdr")));
    }
    folder = Engine::ProgramPath() + "res/skybox/";
    dir = opendir(folder.c_str());
    while (dir != nullptr && (e = readdir(dir)) != nullptr) {
        if (e->d_name[0] == '.') {
            continue;
        }
        std::string name = e->d_name;
        auto newEnv = Environment::create(name);
        try {
            newEnv->set_diffuse(Cubemap::parse(name, folder));
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
            continue;
        }
        try {
            newEnv->set_irradiance(Cubemap::parse(name + "/light", folder));
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    closedir(dir);
    Environment::set_current(Environment::Get(0));
}

void EnginePrivate::Update()
{
    for (auto i = 0; Node::Get(i); i++) {
        auto node = Node::Get(i);
        node->Update();
    }
}

void EnginePrivate::FixedUpdate()
{
    for (auto i = 0; Node::Get(i); i++) {
        auto node = Node::Get(i);
        node->transform_update();
    }
    for (auto i = 0; Node::Get(i); i++) {
        auto node = Node::Get(i);
        node->FixedUpdate();
    }
}

void Engine::Init()
{
    Window::init(Config::WindowName(), Config::WindowSize());
    static auto SSAOShaderCode =
#include "ssao.frag"
        ;
    static auto SSAOShader = GLSL::compile("SSAO", SSAOShaderCode, PostShader);
    Render::AddPostTreatment(SSAOShader);
    EnginePrivate::Get().LoadRes();
}

int event_filter(void* arg, SDL_Event* event)
{
    return (Events::filter(arg, event));
}

void Engine::Start()
{
    float ticks;
    float lastTicks;
    float fixedTiming;

    fixedTiming = lastTicks = SDL_GetTicks() / 1000.f;
    SDL_SetEventFilter(event_filter, nullptr);
    SDL_GL_MakeCurrent(Window::sdl_window(), nullptr);
    Render::Start();
    while (EnginePrivate::Get().loop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        EnginePrivate::Get().updateMutex.lock();
        ticks = SDL_GetTicks() / 1000.f;
        EnginePrivate::Get().deltaTime = ticks - lastTicks;
        lastTicks = ticks;
        SDL_PumpEvents();
        Events::refresh();
        if (ticks - fixedTiming >= 0.015) {
            fixedTiming = ticks;
            EnginePrivate::FixedUpdate();
        }
        EnginePrivate::Update();
        EnginePrivate::Get().updateMutex.unlock();
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

const std::string& Engine::ProgramPath()
{
    return (EnginePrivate::Get().programPath);
}

const std::string& Engine::ExecutionPath()
{
    return (EnginePrivate::Get().execPath);
}

const std::string Engine::ResourcePath()
{
    return (Engine::ProgramPath() + "/res/");
}

std::mutex& Engine::UpdateMutex(void)
{
    return EnginePrivate::Get().updateMutex;
}
