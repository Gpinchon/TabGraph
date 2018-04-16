/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/16 16:45:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 16:46:08 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

VEC2		window_get_size(void)
{
	int	w;
	int	h;

	SDL_GetWindowSize(window_get()->sdl_window, &w, &h);
	return (new_vec2(w, h));
}

void		window_resize(void)
{
	VEC2	size;

	size = vec2_scale(window_get_size(),
		engine_get()->internal_quality);
	framebuffer_resize(window_get()->render_buffer, size);
}

void		window_fullscreen(char fullscreen)
{
	SDL_SetWindowFullscreen(window_get()->sdl_window,
		fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	window_resize();
}
