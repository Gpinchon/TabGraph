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

#include "scop.hpp"

/*void	callback_scale(SDL_Event *event)
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
}*/

void	callback_background(SDL_Event *event)
{
	static int	b = 0;

	if (event && (event->type == SDL_KEYUP || event->key.repeat))
		return ;
	b++;
	Environment	*env;
	if ((env = Engine::environment(b)))
		Engine::current_environment(env);
	else
	{
		b = 0;
		env = Engine::environment(b);
		Engine::current_environment(env);
	}
}

void	callback_quality(SDL_Event *event)
{
	if (event && event->type != SDL_KEYDOWN)
		return ;
	Engine::internal_quality() += 0.25;
	Engine::internal_quality() = CYCLE(Engine::internal_quality(), 0.25, 1.25);
}

void	callback_refresh(SDL_Event *)
{
	static float	rotation = 0;
	auto	mesh = Engine::mesh(0);
	if (!mesh)
		return;
	rotation += 0.2 * Engine::delta_time();
	rotation = CYCLE(rotation, 0, 2 * M_PI);
	mesh->rotation() = new_vec3(0, rotation, 0);
	//std::cout << "refresh\n";
}

void	callback_exit(SDL_Event *)
{
	Engine::stop();
}

void	callback_fullscreen(SDL_Event *event)
{
	static char	fullscreen = 0;
	const Uint8	*s;

	s = SDL_GetKeyboardState(NULL);
	if (s[SDL_SCANCODE_RETURN] && s[SDL_SCANCODE_LALT])
	{
		fullscreen = !fullscreen;
		Window::fullscreen(fullscreen);
	}
	(void)event;
}
