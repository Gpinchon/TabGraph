/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:17:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/02 22:48:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "Mesh.hpp"
#include "Window.hpp"

static inline void	callback_get_v(VEC4 *v0, VEC4 *v1, const std::vector<Uint8> &s)
{
	*v0 = vec4_add(*v0, new_vec4(
		s.at(DOWNK) != 0u ? 0.1 : -0.2,
		s.at(LEFTK) != 0u ? 0.1 : -0.2,
		static_cast<unsigned int>((s.at(UNZOOMK) != 0u) && (s.at(SDL_SCANCODE_LCTRL) == 0u)) != 0u ? 0.1 : -0.2,
		s.at(SDL_SCANCODE_PAGEUP) != 0u ? 0.1 : -0.2));
	*v1 = vec4_add(*v1, new_vec4(
		s.at(UPK) != 0u ? 0.1 : -0.2,
		s.at(RIGHTK) != 0u ? 0.1 : -0.2,
		static_cast<unsigned int>((s.at(ZOOMK) != 0u) && (s.at(SDL_SCANCODE_LCTRL) == 0u)) != 0u ? 0.1 : -0.2,
		s.at(SDL_SCANCODE_PAGEDOWN) != 0u ? 0.1 : -0.2));
	*v0 = new_vec4(CLAMP(v0->x, 0, 1), CLAMP(v0->y, 0, 1),
		CLAMP(v0->z, 0, 1), CLAMP(v0->w, 0, 1));
	*v1 = new_vec4(CLAMP(v1->x, 0, 1), CLAMP(v1->y, 0, 1),
		CLAMP(v1->z, 0, 1), CLAMP(v1->w, 0, 1));
}

auto	keys()
{
	int		length(0);
	auto	keys = SDL_GetKeyboardState(&length);
	std::vector<Uint8>	s;
	s.assign(keys, keys + length);
	return (s);
}

void				callback_camera(SDL_Event */*unused*/)
{
	if (Engine::current_camera() == nullptr) {
		return ;
	}
	static VEC3	val = (VEC3){M_PI / 2.f, M_PI / 2.f, 5.f};
	static VEC4 v[] = {(VEC4){0, 0, 0, 0}, (VEC4){0, 0, 0, 0}};
	float	mv;
	auto	s = keys();
	mv = (1 + s.at(SDL_SCANCODE_LSHIFT)) * Engine::delta_time();
	auto t = Engine::current_camera()->target;
	callback_get_v(&v[0], &v[1], s);
	val.x += (s.at(DOWNK) + v[0].x) * mv + (s.at(UPK) + v[1].x) * -mv;
	val.y += (s.at(LEFTK) + v[0].y) * mv + (s.at(RIGHTK) + v[1].y) * -mv;
	val.z += (static_cast<float>((s.at(UNZOOMK) != 0u) && (s.at(SDL_SCANCODE_LCTRL) == 0u)) + v[0].z) * mv;
	val.z += (static_cast<float>((s.at(ZOOMK) != 0u) && (s.at(SDL_SCANCODE_LCTRL) == 0u)) + v[1].z) * -mv;
	t->position().y += (s.at(SDL_SCANCODE_PAGEUP) + v[0].w) * mv;
	t->position().y += (s.at(SDL_SCANCODE_PAGEDOWN) + v[1].w) * -mv;
	val.x = CLAMP(val.x, 0.01, M_PI - 0.01);
	val.y = CYCLE(val.y, 0, 2 * M_PI);
	val.z = CLAMP(val.z, 0.1f, 1000.f);
	Engine::current_camera()->orbite(val.x, val.y, val.z);
}

void	callback_scale(SDL_Event *event)
{
	static float	scale = 1;

	if (event == nullptr || event->type == SDL_KEYUP) {
		return ;
	}
	auto	mesh = Mesh::get_by_name("main_mesh");
	auto	s = keys();
	if (s.at(SDL_SCANCODE_LCTRL) == 0u) {
		return ;
	}
	if (s.at(SDL_SCANCODE_KP_PLUS) != 0u) {
		scale += (0.005 * (s.at(SDL_SCANCODE_LSHIFT) + 1));
	}
	else if (s.at(SDL_SCANCODE_KP_MINUS) != 0u) {
		scale -= (0.005 * (s.at(SDL_SCANCODE_LSHIFT) + 1));
	}
	scale = CLAMP(scale, 0.0001, 1000);
	mesh->scale() = new_vec3(scale, scale, scale);
}

void	callback_background(SDL_Event *event)
{
	static int	b = 0;

	if (event == nullptr || (event->type == SDL_KEYUP || (event->key.repeat != 0u))) {
		return ;
	}
	b++;
	Environment	*env;
	if ((env = Engine::environment(b)) != nullptr) {
		Engine::current_environment(env);
	}
	else {
		b = 0;
		env = Engine::environment(b);
		Engine::current_environment(env);
	}
}

void	callback_quality(SDL_Event *event)
{
	if (event == nullptr || event->type != SDL_KEYDOWN) {
		return ;
	}
	Engine::internal_quality() += 0.25;
	Engine::internal_quality() = CYCLE(Engine::internal_quality(), 0.5, 1.5);
}

void	callback_refresh(SDL_Event */*unused*/)
{
	static float	rotation = 0;
	auto	mesh = Engine::renderable(0);
	if (mesh == nullptr) {
		return ;
	}
	rotation += 0.2 * Engine::delta_time();
	rotation = CYCLE(rotation, 0, 2 * M_PI);
	mesh->rotation() = new_vec3(0, rotation, 0);
	callback_camera(nullptr);
}

void	callback_exit(SDL_Event */*unused*/)
{
	Engine::stop();
}

void	callback_fullscreen(SDL_Event *event)
{
	static bool	fullscreen = false;
	auto		s = keys();

	if ((s.at(SDL_SCANCODE_RETURN) != 0u) && (s.at(SDL_SCANCODE_LALT) != 0u)) {
		fullscreen = !fullscreen;
		Window::fullscreen(fullscreen);
	}
	(void)event;
}