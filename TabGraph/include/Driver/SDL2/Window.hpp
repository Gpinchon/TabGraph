/*
* @Author: gpinchon
* @Date:   2021-05-19 15:55:01
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 14:37:47
*/
#pragma once

#include <Window.hpp>

#include <glm/vec2.hpp>
#include <array>

struct SDL_Window;

struct Window::Impl {
    using Handle = SDL_Window*;
    Impl(const std::string& name, const glm::ivec2& resolution, const Style style);
    Signal<Event::Window>& OnEvent(const Event::Window::Type type);
    Handle GetHandle() const;
    uint32_t GetId() const;
    glm::ivec2 GetSize() const;
    void SetSize(const glm::ivec2& size);
    void SetFullscreen(bool fullscreen);
    void Swap();

private:
    Handle _handle { nullptr };
    std::array<Signal<Event::Window>, size_t(Event::Window::Type::MaxValue)> _onEvent;
};

struct SDL_WindowEvent;

namespace SDL2 {
namespace Window {
    Event::Window CreateEventData(const SDL_WindowEvent& event);
};
};
