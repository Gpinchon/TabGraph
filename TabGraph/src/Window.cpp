/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 19:39:41
*/

#include <Event/EventsManager.hpp> // for Events
#include <Window.hpp>

#if MEDIALIBRARY == SDL2
#include <Driver/SDL2/Window.hpp>
#endif //MEDIALIBRARY == SDL2

#include <stdexcept> // for runtime_error

static std::unordered_map<uint32_t, std::shared_ptr<Window>> s_windows;
static uint32_t s_currentWindow { 0 };

std::shared_ptr<Window> Window::Create(const std::string& name, const glm::ivec2& resolution, const Style style)
{
    auto window { std::shared_ptr<Window>(new Window(name, resolution, style)) };
    s_windows[window->GetId()] = window;
    return window;
}

std::shared_ptr<Window> Window::Get(uint32_t id)
{
    auto window { s_windows.find(id) };
    return window == s_windows.end() ? nullptr : window->second;
}

std::shared_ptr<Window> Window::GetCurrent()
{
    return Get(s_currentWindow);
}

uint32_t Window::GetId()
{
    return _impl->GetId();
}

Window::Window(const std::string& name, const glm::ivec2 resolution, const Style style)
    : _impl(new Window::Impl(name, resolution, style))
{
    EventsManager::On(Event::Type::WindowEvent).ConnectMember(this, &Window::_ProcessEvent);
}

void Window::_ProcessEvent(const Event& event)
{
    auto& windowEvent { event.Get<Event::Window>() };
    if (windowEvent.type == Event::Window::Type::FocusGained)
        s_currentWindow = windowEvent.window->GetId();
    else if (windowEvent.type == Event::Window::Type::FocusLost
          && s_currentWindow == windowEvent.window->GetId())
        s_currentWindow = 0;
    _impl->OnEvent(windowEvent.type)(windowEvent);
    if (windowEvent.type == Event::Window::Type::Close) {
        s_windows.erase(windowEvent.window->GetId());
        if (s_currentWindow == windowEvent.window->GetId())
            s_currentWindow = 0;
    }
}

Window::Impl& Window::GetImpl()
{
    return *_impl;
}

Signal<Event::Window>& Window::OnEvent(const Event::Window::Type type)
{
    return _impl->OnEvent(type);
}

void Window::Swap()
{
    return _impl->Swap();
}

glm::ivec2 Window::GetSize() const
{
    return _impl->GetSize();
}

void Window::SetSize(const glm::ivec2& size)
{
    return _impl->SetSize(size);
}

void Window::SetFullscreen(const bool& fullscreen)
{
    return _impl->SetFullscreen(fullscreen);
}
