/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:17:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:36:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

void	callback_stupidity(SDL_Event *event)
{
	if (event && event->type == SDL_KEYUP)
		return ;
	engine_get()->new_stupidity = !engine_get()->new_stupidity;
}

void	callback_scale(SDL_Event *event)
{
	const Uint8		*s;
	static float	scale = 1;

	if (!event || (event && event->type == SDL_KEYUP))
		return ;
	s = SDL_GetKeyboardState(NULL);
	if (!s[SDL_SCANCODE_LCTRL])
		return ;
	if (s[SDL_SCANCODE_KP_PLUS])
		scale += (0.005 * (s[SDL_SCANCODE_LSHIFT] + 1));
	else if (s[SDL_SCANCODE_KP_MINUS])
		scale -= (0.005 * (s[SDL_SCANCODE_LSHIFT] + 1));
	scale = CLAMP(scale, 0.0001, 1000);
	mesh_scale(0, new_vec3(scale, scale, scale));
}

void	callback_background(SDL_Event *event)
{
	static int	b = 0;

	if (event && (event->type == SDL_KEYUP || event->key.repeat))
		return ;
	b = CYCLE(b + 1, 0, (int)engine_get()->textures_env.length / 2);
	engine_get()->env = *((int*)ezarray_get_index(
		engine_get()->textures_env, b * 2 + 0));
	engine_get()->env_spec = *((int*)ezarray_get_index(
		engine_get()->textures_env, b * 2 + 1));
}

void	callback_exit(SDL_Event *event)
{
	engine_get()->loop = 0;
	(void)event;
}

void	callback_fullscreen(SDL_Event *event)
{
	static char	fullscreen = 0;
	const Uint8	*s;

	s = SDL_GetKeyboardState(NULL);
	if (s[SDL_SCANCODE_RETURN] && s[SDL_SCANCODE_LALT])
	{
		fullscreen = !fullscreen;
		window_fullscreen(fullscreen);
	}
	(void)event;
}
