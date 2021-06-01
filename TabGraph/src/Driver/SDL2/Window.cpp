/*
* @Author: gpinchon
* @Date:   2021-05-22 14:36:20
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-24 23:45:06
*/

#include <Driver/SDL2/Window.hpp>

#include <SDL.h>
#include <SDL_error.h>
#include <SDL_video.h>

#include <stdexcept>

Window::Impl::Impl(const std::string& name, const glm::ivec2& resolution, const Style style)
{
    if (!SDL_WasInit(SDL_INIT_VIDEO) && SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
    }
    auto windowFlags { SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI };
    if ((style & Style::None) == Style::None)
        windowFlags |= SDL_WINDOW_BORDERLESS;
    //else if ((style & Style::Titlebar) == Style::Titlebar) Not managed by SDL2;
    else if ((style & Style::Resize) == Style::Resize)
        windowFlags |= SDL_WINDOW_RESIZABLE;
    //else if ((style & Style::Close) == Style::Close) Not managed by SDL2;
    if ((style & Style::Fullscreen) == Style::Fullscreen)
        windowFlags |= SDL_WINDOW_FULLSCREEN;
#if RENDERINGAPI == OpenGL
    windowFlags |= SDL_WINDOW_OPENGL; //enable OpenGL capabilities
#endif //RENDERINGAPI == OpenGL
    _handle = SDL_CreateWindow(name.c_str(), SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, windowFlags);
    if (GetHandle() == nullptr) {
        throw std::runtime_error(SDL_GetError());
    }
}

Signal<Event::Window>& Window::Impl::OnEvent(const Event::Window::Type type)
{
    return _onEvent.at(size_t(type));
}

Window::Impl::Handle Window::Impl::GetHandle() const
{
    return _handle;
}

uint32_t Window::Impl::GetId() const
{
    return SDL_GetWindowID(_handle);
}

glm::ivec2 Window::Impl::GetSize() const
{
    glm::ivec2 size { 0 };
#if RENDERINGAPI == OpenGL
    SDL_GL_GetDrawableSize((SDL_Window*)GetHandle(), &size.x, &size.y);
#endif //RENDERINGAPI == OpenGL
    return size;
}

void Window::Impl::SetSize(const glm::ivec2& size)
{
    SDL_SetWindowSize((SDL_Window*)GetHandle(), size.x, size.y);
}

void Window::Impl::SetFullscreen(bool fullscreen)
{
    SDL_SetWindowFullscreen((SDL_Window*)GetHandle(),
        fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}

void Window::Impl::Swap()
{
#if RENDERINGAPI == OpenGL
    SDL_GL_SwapWindow((SDL_Window*)GetHandle());
#endif //RENDERINGAPI == OpenGL
}

namespace SDL2 {
namespace Window {
    Event::Window::Type GetEventType(SDL_WindowEventID type)
    {
        switch (type) {
        case SDL_WINDOWEVENT_NONE:
            return Event::Window::Type::Unknown;
        case SDL_WINDOWEVENT_SHOWN:
            return Event::Window::Type::Shown;
        case SDL_WINDOWEVENT_HIDDEN:
            return Event::Window::Type::Hidden;
        case SDL_WINDOWEVENT_EXPOSED:
            return Event::Window::Type::Exposed;
        case SDL_WINDOWEVENT_MOVED:
            return Event::Window::Type::Moved;
        case SDL_WINDOWEVENT_RESIZED:
            return Event::Window::Type::Resized;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            return Event::Window::Type::SizeChanged;
        case SDL_WINDOWEVENT_MINIMIZED:
            return Event::Window::Type::Minimized;
        case SDL_WINDOWEVENT_MAXIMIZED:
            return Event::Window::Type::Maximized;
        case SDL_WINDOWEVENT_RESTORED:
            return Event::Window::Type::Restored;
        case SDL_WINDOWEVENT_ENTER:
            return Event::Window::Type::Enter;
        case SDL_WINDOWEVENT_LEAVE:
            return Event::Window::Type::Leave;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            return Event::Window::Type::FocusGained;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            return Event::Window::Type::FocusLost;
        case SDL_WINDOWEVENT_CLOSE:
            return Event::Window::Type::Close;
        case SDL_WINDOWEVENT_TAKE_FOCUS:
            return Event::Window::Type::TakeFocus;
        default:
            throw std::runtime_error("Unknown Event::Window::Type");
        }
    }
    Event::Window CreateEventData(const SDL_WindowEvent& SDLevent)
    {
        Event::Window event;
        event.type = GetEventType(SDL_WindowEventID(SDLevent.event));
        event.window = ::Window::Get(SDLevent.windowID);
        return event;
    }
}
}


