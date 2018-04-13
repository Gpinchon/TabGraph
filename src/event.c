/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 17:24:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:36:06 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

void	event_window(SDL_Event *event)
{
	if (event->window.event == SDL_WINDOWEVENT_CLOSE)
		engine_get()->loop = 0;
	else if (event->window.event == SDL_WINDOWEVENT_RESIZED)
		window_resize();
}

void	event_keyboard(SDL_Event *event)
{
	if (engine_get()->kcallbacks[event->key.keysym.scancode])
		engine_get()->kcallbacks[event->key.keysym.scancode](event);
}

int		event_callback(void *e, SDL_Event *event)
{
	t_engine *engine;

	engine = e;
	if (event->type == SDL_QUIT)
		engine->loop = 0;
	else if (event->type == SDL_WINDOWEVENT)
		event_window(event);
	else if (event->type == SDL_KEYUP
		|| event->type == SDL_KEYDOWN)
		event_keyboard(event);
	return (0);
}

int		event_refresh(void)
{
	if (engine_get()->rcallback)
		engine_get()->rcallback(NULL);
	return (0);
}
