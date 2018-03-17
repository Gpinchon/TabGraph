/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   window.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:22:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 14:52:28 by gpinchon         ###   ########.fr       */
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

void		window_fullscreen(char fullscreen)
{
	SDL_SetWindowFullscreen(window_get()->sdl_window,
		fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
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
	window->render_buffer = framebuffer_create(new_vec2(IWIDTH, IHEIGHT), shader_get_by_name("render"), 4, 1);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
