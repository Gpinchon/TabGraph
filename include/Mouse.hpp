/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mouse.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/07 18:08:40 by gpinchon          #+#    #+#             */
/*   Updated: 2018/11/14 23:49:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "InputDevice.hpp"
#include <array>

typedef void (*mouse_motion_callback)(SDL_MouseMotionEvent* event);
typedef void (*mouse_button_callback)(SDL_MouseButtonEvent* event);
typedef void (*mouse_wheel_callback)(SDL_MouseWheelEvent* event);

class Mouse : InputDevice {
public:
    void process_event(SDL_Event*);
    static bool set_relative(SDL_bool relative);
    static bool button(Uint8 button);
    static void position(Sint32& x, Sint32& y);
    static void set_move_callback(mouse_motion_callback);
    static void set_wheel_callback(mouse_wheel_callback);
    static void set_button_callback(mouse_button_callback, Uint8 button);

private:
    Mouse();
    static Mouse* _get();
    static Mouse* _instance;
    std::array<mouse_button_callback, 5> _button_callbacks { { nullptr } };
    mouse_motion_callback _move_callback { nullptr };
    mouse_wheel_callback _wheel_callback { nullptr };
};