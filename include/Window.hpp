/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Window.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:50:23 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 00:12:50 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "vml.h"

class	Window
{
public :
	static void		resize();
	static VEC2		size();
	static VEC2		internal_resolution();
	static void		init(const std::string &name, int width, int height);
	static void		fullscreen(const bool &fullscreen);
	static void		swap();
	static GLbitfield	&clear_mask();
	static VEC4			&clear_color();
	static Framebuffer	&render_buffer();
private :
	static Window	&_get();
	static Window	*_instance;
	SDL_Window	    *_sdl_window{nullptr};
	SDL_GLContext	_gl_context{};
	VEC4		    _clear_color{0, 0, 0, 0};
	GLbitfield  	_clear_mask{};
	Framebuffer		*_render_buffer{nullptr};
	Window();
};