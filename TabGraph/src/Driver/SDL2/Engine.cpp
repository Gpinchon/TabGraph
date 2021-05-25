/*
* @Author: gpinchon
* @Date:   2021-05-24 13:38:06
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 13:44:07
*/

#include <Driver/SDL2/Engine.hpp>
#include <Event/EventsManager.hpp>
#include <Renderer/Renderer.hpp>

#include <SDL_filesystem.h>
#include <SDL_timer.h>
#include <SDL_video.h>

Engine::Impl::Impl(std::shared_ptr<Renderer::FrameRenderer> frameRenderer)
    : _frameRenderer(frameRenderer)
{
    EventsManager::Add(this, Event::Type::Quit);
}

std::filesystem::path Engine::Impl::GetProgramPath()
{
    return std::filesystem::absolute(SDL_GetBasePath()).parent_path();
}

std::filesystem::path Engine::Impl::GetExecutionPath()
{
    return std::filesystem::current_path();
}

void Engine::Impl::ProcessEvent(const Event&)
{
    Stop();
}

void Engine::Impl::Start()
{
    double ticks;
    double lastTicks;
    double fixedTiming = lastTicks = SDL_GetTicks() / 1000.f;

    while (_loop) {
        ticks = SDL_GetTicks() / 1000.0;
        EventsManager::PollEvents();
        _onUpdate(ticks - lastTicks);
        if (ticks - fixedTiming >= 0.015) {
            _onFixedUpdate(ticks - fixedTiming);
            fixedTiming = ticks;
        }
        _frameRenderer->RenderFrame(_currentScene);
        lastTicks = ticks;
    }
}