/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 19:26:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Engine.hpp"

const auto WIDTH =		1280;
const auto HEIGHT =		720;

/*
** Key binding
*/

# define DOWNK		SDL_SCANCODE_DOWN
# define UPK		SDL_SCANCODE_UP
# define LEFTK		SDL_SCANCODE_LEFT
# define RIGHTK		SDL_SCANCODE_RIGHT
# define ZOOMK		SDL_SCANCODE_KP_PLUS
# define UNZOOMK	SDL_SCANCODE_KP_MINUS

//bool			load_mtllib(const std::string &path);

/*
** Callback functions
*/

void			callback_refresh(SDL_Event *event);
void			callback_camera(SDL_Event *event);
void			callback_scale(SDL_KeyboardEvent *event);
void			callback_background(SDL_KeyboardEvent *event);
void			callback_exit(SDL_KeyboardEvent *event);
void			callback_fullscreen(SDL_KeyboardEvent *event);
void			callback_quality(SDL_KeyboardEvent *event);
void			controller_callback_background(SDL_ControllerButtonEvent *event);
void			controller_callback_quality(SDL_ControllerButtonEvent *event);
void			controller_callback_rotation(SDL_ControllerButtonEvent *event);
void			keyboard_callback_rotation(SDL_KeyboardEvent *event);
void			MouseWheelCallback(SDL_MouseWheelEvent *event);
void			MouseMoveCallback(SDL_MouseMotionEvent *event);
