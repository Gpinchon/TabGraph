/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:35
*/

#pragma once

#include "Event/InputDevice.hpp" // for InputDevice
#include "glm/glm.hpp" // for glm::vec2, glm::vec4, s_vec2
#include <GL/glew.h> // for GLbitfield
#include <SDL_events.h> // for SDL_Event
#include <SDL_video.h> // for SDL_GLContext, SDL_Window
#include <string> // for string

class Window : InputDevice {
public:
    static void resize(const glm::ivec2&);
    static glm::ivec2 size();
    static void init(const std::string& name, glm::ivec2 resolution);
    static void fullscreen(const bool& fullscreen);
    static void swap();
    static GLbitfield& clear_mask();
    static glm::vec4& clear_color();
    //static Framebuffer	&render_buffer();
    void process_event(SDL_Event*) override;
    static SDL_GLContext context();
    static SDL_Window* sdl_window();

private:
    static Window& _get();
    static Window* _instance;
    SDL_Window* _sdl_window { nullptr };
    SDL_GLContext _gl_context {};
    glm::vec4 _clear_color { 0, 0, 0, 0 };
    GLbitfield _clear_mask {};
    //Framebuffer		*_render_buffer{nullptr};
    Window();
};