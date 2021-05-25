/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 15:10:44
*/

#pragma once

#include <Event/InputDevice/InputDevice.hpp>
#include <Event/Event.hpp>
#include <Event/Signal.hpp>

#include <glm/vec2.hpp>
#include <memory>
#include <string>

struct Window : InputDevice {
    class Impl;
    enum class Style {
        None = 0x1, //Window has no decoration
        Titlebar = 0x2, //Window has a titlebar
        Resize = 0x4, //Window can be resized and has a maximize button
        Close = 0x8, //Window has a close button
        Fullscreen = 0x10, //Window is shown in fullscreen mode; this style cannot be combined with others, and requires a valid video mode
        Default = 0xe //Titlebar | Resize | Close
    };
    static std::shared_ptr<Window> Create(const std::string& name, const glm::ivec2& resolution, const Style style = Style::Default);
    /**
     * @param id : the id of the Window to query
     * @return a pointer to the Window corresponding to the id, nullptr if no Window corresponds to id
    */
    static std::shared_ptr<Window> Get(uint32_t id);
    /**
     * @return the last Window who took the focus, nullptr if no Window took the focus
    */
    static std::shared_ptr<Window> GetCurrent();
    uint32_t GetId();
    void SetSize(const glm::ivec2&);
    glm::ivec2 GetSize() const;
    void SetFullscreen(const bool& fullscreen);
    void Swap();
    virtual void ProcessEvent(const Event& event) override;
    Impl& GetImpl();
    Signal<Event::Window>& OnEvent(const Event::Window::Type type);

private:
    Window(const std::string& name, const glm::ivec2 resolution, const Style style);
    std::unique_ptr<Impl> _impl;
};

static inline auto operator|(const Window::Style a, const Window::Style b) {
    return static_cast<Window::Style>(static_cast<int>(a) | static_cast<int>(b));
}

static inline auto operator&(const Window::Style a, const Window::Style b) {
    return static_cast<Window::Style>(static_cast<int>(a) & static_cast<int>(b));
}
