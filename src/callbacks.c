/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:17:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/13 00:43:05 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>


void	callback_stupidity(SDL_Event *event)
{
	if (event && event->type == SDL_KEYUP)
		return;
	engine_get()->new_stupidity = !engine_get()->new_stupidity;
}

void	callback_scale(SDL_Event *event)
{
	const Uint8		*state;
	static float	scale = 1;

	if (!event || (event && event->type == SDL_KEYUP))
		return;
	state = SDL_GetKeyboardState(NULL);
	if (!state[SDL_SCANCODE_LCTRL])
		return;
	if (state[SDL_SCANCODE_KP_PLUS])
		scale += (0.005 * (state[SDL_SCANCODE_LSHIFT] + 1));
	else if (state[SDL_SCANCODE_KP_MINUS])
		scale -= (0.005 * (state[SDL_SCANCODE_LSHIFT] + 1));
	scale = CLAMP(scale, 0.0001, 1000);
	mesh_scale(0, new_vec3(scale, scale, scale));
	(void)event;
}

void	callback_background(SDL_Event *event)
{
	static int	b = 0;

	if (event && (event->type == SDL_KEYUP || event->key.repeat))
		return;
	b = CYCLE(b + 1, 0, (int)engine_get()->textures_env.length / 2 - 1);
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
	const Uint8	*state;

	state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_RETURN] && state[SDL_SCANCODE_LALT])
	{
		fullscreen = !fullscreen;
		window_fullscreen(fullscreen);
	}
	(void)event;
}

void	callback_camera(SDL_Event *e)
{
	static VEC3	val = (VEC3){M_PI / 2.f, M_PI / 2.f, 5.f};
	static VEC3 velocity[] = {(VEC3){0, 0, 0}, (VEC3){0, 0, 0}};
	float		mv;
	const Uint8	*state;
	t_transform	*t;

	state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_DOWN])
		velocity[0].x = CLAMP(velocity[0].x + 0.1, 0, 1);
	else
		velocity[0].x = CLAMP(velocity[0].x - 0.2, 0, 1);
	if (state[SDL_SCANCODE_UP])
		velocity[1].x = CLAMP(velocity[1].x + 0.1, 0, 1);
	else
		velocity[1].x = CLAMP(velocity[1].x - 0.2, 0, 1);
	if (state[SDL_SCANCODE_LEFT])
		velocity[0].y = CLAMP(velocity[0].y + 0.1, 0, 1);
	else
		velocity[0].y = CLAMP(velocity[0].y - 0.2, 0, 1);
	if (state[SDL_SCANCODE_RIGHT])
		velocity[1].y = CLAMP(velocity[1].y + 0.1, 0, 1);
	else
		velocity[1].y = CLAMP(velocity[1].y - 0.2, 0, 1);
	if (state[SDL_SCANCODE_KP_MINUS] && !state[SDL_SCANCODE_LCTRL])
		velocity[0].z = CLAMP(velocity[0].z + 0.1, 0, 1);
	else
		velocity[0].z = CLAMP(velocity[0].z - 0.2, 0, 1);
	if (state[SDL_SCANCODE_KP_PLUS] && !state[SDL_SCANCODE_LCTRL])
		velocity[1].z = CLAMP(velocity[1].z + 0.1, 0, 1);
	else
		velocity[1].z = CLAMP(velocity[1].z - 0.2, 0, 1);
	mv = (1 + state[SDL_SCANCODE_LSHIFT]) * engine_get()->delta_time;
	t = ezarray_get_index(engine_get()->transforms,
		camera_get_target_index(0));
	val.x += (state[SDL_SCANCODE_DOWN] + velocity[0].x) * mv;
	val.x += (state[SDL_SCANCODE_UP] + velocity[1].x) * -mv;
	val.y += (state[SDL_SCANCODE_LEFT] + velocity[0].y) * mv;
	val.y += (state[SDL_SCANCODE_RIGHT] + velocity[1].y) * -mv;
	val.z += (state[SDL_SCANCODE_KP_MINUS] + velocity[0].z) * mv;
	val.z += (state[SDL_SCANCODE_KP_PLUS] + velocity[1].z) * -mv;
	t->position.y += state[SDL_SCANCODE_PAGEUP] * mv;
	t->position.y += state[SDL_SCANCODE_PAGEDOWN] * -mv;	
	val.x = CLAMP(val.x, 0.01, M_PI - 0.01);
	val.y = CYCLE(val.y, 0, 2 * M_PI);
	val.z = CLAMP(val.z, 0.1f, 1000.f);
	camera_orbite(0, val.x, val.y, val.z);
	(void)e;
}
