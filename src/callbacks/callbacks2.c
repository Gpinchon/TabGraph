/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/25 19:43:08 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 17:15:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

static inline void	callback_get_v(VEC4 *v0, VEC4 *v1, const Uint8 *s)
{
	*v0 = vec4_add(*v0, new_vec4(
		s[DOWNK] ? 0.1 : -0.2,
		s[LEFTK] ? 0.1 : -0.2,
		(s[UNZOOMK] && !s[SDL_SCANCODE_LCTRL]) ? 0.1 : -0.2,
		s[SDL_SCANCODE_PAGEUP] ? 0.1 : -0.2));
	*v1 = vec4_add(*v1, new_vec4(
		s[UPK] ? 0.1 : -0.2,
		s[RIGHTK] ? 0.1 : -0.2,
		(s[ZOOMK] && !s[SDL_SCANCODE_LCTRL]) ? 0.1 : -0.2,
		s[SDL_SCANCODE_PAGEDOWN] ? 0.1 : -0.2));
	*v0 = new_vec4(CLAMP(v0->x, 0, 1), CLAMP(v0->y, 0, 1),
		CLAMP(v0->z, 0, 1), CLAMP(v0->w, 0, 1));
	*v1 = new_vec4(CLAMP(v1->x, 0, 1), CLAMP(v1->y, 0, 1),
		CLAMP(v1->z, 0, 1), CLAMP(v1->w, 0, 1));
}

void				callback_camera(SDL_Event *e)
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
	callback_get_v(&v[0], &v[1], s);
	val.x += (s[DOWNK] + v[0].x) * mv + (s[UPK] + v[1].x) * -mv;
	val.y += (s[LEFTK] + v[0].y) * mv + (s[RIGHTK] + v[1].y) * -mv;
	val.z += ((s[UNZOOMK] && !s[SDL_SCANCODE_LCTRL]) + v[0].z) * mv;
	val.z += ((s[ZOOMK] && !s[SDL_SCANCODE_LCTRL]) + v[1].z) * -mv;
	t->position.y += (s[SDL_SCANCODE_PAGEUP] + v[0].w) * mv;
	t->position.y += (s[SDL_SCANCODE_PAGEDOWN] + v[1].w) * -mv;
	val.x = CLAMP(val.x, 0.01, M_PI - 0.01);
	val.y = CYCLE(val.y, 0, 2 * M_PI);
	val.z = CLAMP(val.z, 0.1f, 1000.f);
	camera_orbite(0, val.x, val.y, val.z);
	(void)e;
}

void				callback_refresh(SDL_Event *event)
{
	t_material		*m;
	int				i;
	static VEC3		rotation = (VEC3){0, 0, 0};
	static float	val = 0;

	if (engine_get()->stupidity != engine_get()->new_stupidity)
	{
		val = CLAMP(val + 0.1, 0, 1);
		engine_get()->stupidity = interp_cubic(!engine_get()->new_stupidity,
			engine_get()->new_stupidity, val);
		i = 0;
		while ((m = ezarray_get_index(engine_get()->materials, i)))
		{
			m->data.stupidity = engine_get()->stupidity;
			i++;
		}
	}
	else
		val = 0;
	rotation.y = CYCLE(rotation.y + 0.1 * engine_get()->delta_time,
		0, 2 * M_PI);
	callback_camera(NULL);
	mesh_rotate(0, rotation);
	(void)event;
}

void				callback_quality(SDL_Event *e)
{
	if (e && (e->type == SDL_KEYUP || e->key.repeat))
		return ;
	engine_get()->internal_quality += 0.25f;
	engine_get()->internal_quality =
	CYCLE(engine_get()->internal_quality, 0.5, 1.25);
	window_resize();
}
