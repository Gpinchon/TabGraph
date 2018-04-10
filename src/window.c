/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:22:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/09 18:55:49 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

t_window		*window_get()
{
	static t_window	*window = NULL;

	if (!window)
		window = ft_memalloc(sizeof(t_window));
	return (window);
}

VEC2		window_get_size()
{
	int	w;
	int	h;

	SDL_GetWindowSize(window_get()->sdl_window, &w, &h);
	return (new_vec2(w, h));
}

void		window_resize()
{
	VEC2	size = vec2_scale(window_get_size(),
		engine_get()->internal_quality);
	framebuffer_resize(window_get()->render_buffer, size);
}

void		window_fullscreen(char fullscreen)
{
	SDL_SetWindowFullscreen(window_get()->sdl_window,
		fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	window_resize();
}

/*
** window is a singleton
*/
void		window_init(const char *name, int width, int height)
{
	t_window	*window;

	window = window_get();
	if (!window)
		return;
	window->sdl_window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	window->gl_context = SDL_GL_CreateContext(window->sdl_window);
	glewExperimental = GL_TRUE;
	if (!window->sdl_window || glewInit() != GLEW_OK)
		return;
	window->render_buffer = framebuffer_create(vec2_scale(window_get_size(),
		engine_get()->internal_quality), shader_get_by_name("render"), 0, 0);
	framebuffer_create_attachement(window->render_buffer, GL_RGBA, GL_RGBA16F_ARB);
	framebuffer_create_attachement(window->render_buffer, GL_RGB, GL_RGB16F_ARB);
	framebuffer_create_attachement(window->render_buffer, GL_RGB, GL_RGB16F_ARB);
	framebuffer_create_attachement(window->render_buffer, GL_RGB, GL_RGB32F_ARB);
	framebuffer_create_attachement(window->render_buffer, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24);
	framebuffer_setup_attachements(window->render_buffer);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
