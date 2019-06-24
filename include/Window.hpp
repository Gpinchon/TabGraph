/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:25:36
*/

#pragma once

#include <GL/glew.h>          // for GLbitfield
#include <SDL2/SDL_events.h>  // for SDL_Event
#include <SDL2/SDL_video.h>   // for SDL_GLContext, SDL_Window
#include <string>             // for string
#include "InputDevice.hpp"    // for InputDevice
#include "vml.h"              // for VEC2, VEC4, s_vec2

class Window : InputDevice {
public:
    static void resize(const VEC2&);
    static VEC2 size();
    static void init(const std::string& name, VEC2 resolution);
    static void fullscreen(const bool& fullscreen);
    static void swap();
    static GLbitfield& clear_mask();
    static VEC4& clear_color();
    //static Framebuffer	&render_buffer();
    void process_event(SDL_Event*) override;
    static SDL_GLContext   context();
    static SDL_Window      *sdl_window();

private:
    static Window& _get();
    static Window* _instance;
    SDL_Window* _sdl_window{ nullptr };
    SDL_GLContext _gl_context{};
    VEC4 _clear_color{ 0, 0, 0, 0 };
    GLbitfield _clear_mask{};
    //Framebuffer		*_render_buffer{nullptr};
    Window();
};