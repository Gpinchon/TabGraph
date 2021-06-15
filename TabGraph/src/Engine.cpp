/*
* @Author: gpinchon
* @Date:   2021-01-08 17:02:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 13:44:38
*/

#include <Config.hpp> // for Config
#include <Engine.hpp>
#include <Event/EventsManager.hpp> // for Events
#include <Node.hpp> // for Node
#include <Renderer/Renderer.hpp> // for AddPostTreatment, RequestRedraw
#include <Scene/Scene.hpp>
#include <Window.hpp> // for Window

#include <atomic> // for atomic
#include <chrono> // for milliseconds
#include <filesystem>
#include <iostream> // for operator<<, endl, basic_ostream
#include <memory> // for shared_ptr, __shared_ptr_access
#include <thread> // for sleep_for

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/Engine.hpp>
#endif //MEDIALIBRARY == SDL2

std::shared_ptr<Engine> Engine::Create(std::shared_ptr<Renderer::FrameRenderer> frameRenderer)
{
    std::shared_ptr<Engine> engine(new Engine(frameRenderer));
    return engine;
}

Engine::Engine(std::shared_ptr<Renderer::FrameRenderer> frameRenderer)
    : _impl(new Engine::Impl(frameRenderer))
{
}

void Engine::SetCurrentScene(std::shared_ptr<Scene> scene)
{
    return _impl->SetCurrentScene(scene);
}
std::shared_ptr<Scene> Engine::GetCurrentScene() const
{
    return _impl->GetCurrentScene();
}

void Engine::Start()
{
    return _impl->Start();
}

void Engine::Stop(void)
{
    return _impl->Stop();
}

const std::filesystem::path Engine::GetProgramPath()
{
    return Impl::GetProgramPath();
}

const std::filesystem::path Engine::GetExecutionPath()
{
    return Impl::GetExecutionPath();
}

const std::filesystem::path Engine::GetResourcePath()
{
    return (GetProgramPath() / "res");
}

void Engine::SetFrameRenderer(std::shared_ptr<Renderer::FrameRenderer> frameRenderer)
{
    return _impl->SetFrameRenderer(frameRenderer);
}

std::shared_ptr<Renderer::FrameRenderer> Engine::GetFrameRenderer() const
{
    return _impl->GetFrameRenderer();
}

Signal<float>& Engine::OnFixedUpdate()
{
    return _impl->OnFixedUpdate();
}

Signal<float>& Engine::OnUpdate()
{
    return _impl->OnUpdate();
}
