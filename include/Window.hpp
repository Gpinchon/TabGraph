/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:50:23 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/13 18:50:23 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "vml.h"
#include "GLIncludes.hpp"

class	Window
{
public :
	static void		resize();
	static VEC2		size();
	static void		init(const std::string &name, int width, int height);
	static void		fullscreen(const bool &fullscreen);
	static void		swap();
	static GLbitfield	&clear_mask();
	static VEC4			&clear_color();
	static Framebuffer	&render_buffer();
private :
	static Window	&_get();
	SDL_Window	*_sdl_window;
	GLCONTEXT	_gl_context;
	VEC4		_clear_color;
	GLbitfield	_clear_mask;
	Framebuffer		*_render_buffer;
	Window() {};
};