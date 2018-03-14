/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:17:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/14 18:19:18 by gpinchon         ###   ########.fr       */
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
	const Uint8		*s;
	static float	scale = 1;

	if (!event || (event && event->type == SDL_KEYUP))
		return;
	s = SDL_GetKeyboardState(NULL);
	if (!s[SDL_SCANCODE_LCTRL])
		return;
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
	const Uint8	*s;

	s = SDL_GetKeyboardState(NULL);
	if (s[SDL_SCANCODE_RETURN] && s[SDL_SCANCODE_LALT])
	{
		fullscreen = !fullscreen;
		window_fullscreen(fullscreen);
	}
	(void)event;
}

void	callback_camera(SDL_Event *e)
{
	static VEC3	val = (VEC3){M_PI / 2.f, M_PI / 2.f, 5.f};
	static VEC4 v[] = {(VEC4){0, 0, 0, 0}, (VEC4){0, 0, 0, 0}};
	float		mv;
	const Uint8	*s;
	t_transform	*t;

	s = SDL_GetKeyboardState(NULL);
	mv = (1 + s[SDL_SCANCODE_LSHIFT]) * engine_get()->delta_time;
	t = ezarray_get_index(engine_get()->transforms,
		camera_get_target_index(0));
	v[0] = vec4_add(v[0], new_vec4(
		s[SDL_SCANCODE_DOWN] ? 0.1 : -0.2,
		s[SDL_SCANCODE_LEFT] ? 0.1 : -0.2,
		(s[SDL_SCANCODE_KP_MINUS] && !s[SDL_SCANCODE_LCTRL]) ? 0.1 : -0.2,
		s[SDL_SCANCODE_PAGEUP] ? 0.1 : -0.2));
	v[1] = vec4_add(v[1], new_vec4(
		s[SDL_SCANCODE_UP] ? 0.1 : -0.2,
		s[SDL_SCANCODE_RIGHT] ? 0.1 : -0.2,
		(s[SDL_SCANCODE_KP_PLUS] && !s[SDL_SCANCODE_LCTRL]) ? 0.1 : -0.2,
		s[SDL_SCANCODE_PAGEDOWN] ? 0.1 : -0.2));
	v[0] = new_vec4(CLAMP(v[0].x, 0, 1), CLAMP(v[0].y, 0, 1), CLAMP(v[0].z, 0, 1), CLAMP(v[0].w, 0, 1));
	v[1] = new_vec4(CLAMP(v[1].x, 0, 1), CLAMP(v[1].y, 0, 1), CLAMP(v[1].z, 0, 1), CLAMP(v[1].w, 0, 1));
	val.x += (s[SDL_SCANCODE_DOWN] + v[0].x) * mv;
	val.x += (s[SDL_SCANCODE_UP] + v[1].x) * -mv;
	val.y += (s[SDL_SCANCODE_LEFT] + v[0].y) * mv;
	val.y += (s[SDL_SCANCODE_RIGHT] + v[1].y) * -mv;
	val.z += ((s[SDL_SCANCODE_KP_MINUS] && !s[SDL_SCANCODE_LCTRL]) + v[0].z) * mv;
	val.z += ((s[SDL_SCANCODE_KP_PLUS] && !s[SDL_SCANCODE_LCTRL]) + v[1].z) * -mv;
	t->position.y += (s[SDL_SCANCODE_PAGEUP] + v[0].w) * mv;
	t->position.y += (s[SDL_SCANCODE_PAGEDOWN] + v[1].w) * -mv;
	val.x = CLAMP(val.x, 0.01, M_PI - 0.01);
	val.y = CYCLE(val.y, 0, 2 * M_PI);
	val.z = CLAMP(val.z, 0.1f, 1000.f);
	camera_orbite(0, val.x, val.y, val.z);
	(void)e;
}
