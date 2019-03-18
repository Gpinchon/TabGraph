/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:50:23 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/22 19:33:51 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "InputDevice.hpp"
#include <string>

class Window : InputDevice {
public:
    static void resize(const VEC2&);
    static VEC2 size();
    static VEC2 internal_resolution();
    static void init(const std::string& name, VEC2 resolution);
    static void fullscreen(const bool& fullscreen);
    static void swap();
    static GLbitfield& clear_mask();
    static VEC4& clear_color();
    //static Framebuffer	&render_buffer();
    void process_event(SDL_Event*);
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