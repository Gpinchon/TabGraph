/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:22:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 16:47:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

/*
** window is a singleton
*/

t_window	*window_get(void)
{
	static t_window	*window = NULL;

	if (!window)
		window = calloc(1, sizeof(t_window));
	return (window);
}

static void	window_init_framebuffer(void)
{
	window_get()->render_buffer = framebuffer_create(
		vec2_scale(window_get_size(),
		engine_get()->internal_quality), shader_get_by_name("render"), 0, 0);
	framebuffer_create_attachement(window_get()->render_buffer,
		GL_RGBA, GL_RGBA16F_ARB);
	framebuffer_create_attachement(window_get()->render_buffer,
		GL_RGB, GL_RGB16F_ARB);
	framebuffer_create_attachement(window_get()->render_buffer,
		GL_RGB, GL_RGB16F_ARB);
	framebuffer_create_attachement(window_get()->render_buffer,
		GL_RGB, GL_RGB32F_ARB);
	framebuffer_create_attachement(window_get()->render_buffer,
		GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24);
	framebuffer_setup_attachements(window_get()->render_buffer);
}

void		window_init(const char *name, int width, int height)
{
	t_window	*window;

	window = window_get();
	if (!window)
		return ;
	window->sdl_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN |
		SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	window->gl_context = SDL_GL_CreateContext(window->sdl_window);
	glewExperimental = GL_TRUE;
	if (!window->sdl_window || glewInit() != GLEW_OK)
		return ;
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	window_init_framebuffer();
}
