/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:17:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/07 19:38:48 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "Mesh.hpp"
#include "Window.hpp"
#include "Keyboard.hpp"
#include "GameController.hpp"

void				callback_camera(SDL_Event *)
{
	auto	controller = GameController::get(0);
	VEC2	raxis = new_vec2(0, 0);
	VEC2	laxis = new_vec2(0, 0);
	float	ltrigger = 0;
	float	rtrigger = 0;
	if (controller->is_connected()) {
		raxis.x = -controller->axis(SDL_CONTROLLER_AXIS_RIGHTX);
		raxis.y = -controller->axis(SDL_CONTROLLER_AXIS_RIGHTY);
		laxis.x = -controller->axis(SDL_CONTROLLER_AXIS_LEFTX);
		laxis.y = -controller->axis(SDL_CONTROLLER_AXIS_LEFTY);
		ltrigger = controller->axis(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
		rtrigger = controller->axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	}
	else
	{
		raxis.x = Keyboard::key(LEFTK) - Keyboard::key(RIGHTK);
		raxis.y = Keyboard::key(DOWNK) - Keyboard::key(UPK);
		laxis.x = 0;
		laxis.y = Keyboard::key(ZOOMK) - Keyboard::key(UNZOOMK);
		ltrigger = Keyboard::key(SDL_SCANCODE_PAGEDOWN);
		rtrigger = Keyboard::key(SDL_SCANCODE_PAGEUP);
	}
	static VEC3	val = (VEC3){M_PI / 2.f, M_PI / 2.f, 5.f};
	val.x += raxis.y * Engine::delta_time();
	val.y += raxis.x * Engine::delta_time();
	val.z -= laxis.y * Engine::delta_time();
	auto t = Engine::current_camera()->target;
	t->position().y += rtrigger * Engine::delta_time();
	t->position().y -= ltrigger * Engine::delta_time();
	val.x = CLAMP(val.x, 0.01, M_PI - 0.01);
	val.y = CYCLE(val.y, 0, 2 * M_PI);
	val.z = CLAMP(val.z, 0.1f, 1000.f);
	Engine::current_camera()->orbite(val.x, val.y, val.z);
}

void	callback_scale(SDL_KeyboardEvent *event)
{
	static float	scale = 1;

	if (event == nullptr || event->type == SDL_KEYUP) {
		return ;
	}
	auto	mesh = Mesh::get_by_name("main_mesh");
	if (Keyboard::key(SDL_SCANCODE_LCTRL) == 0u) {
		return ;
	}
	if (Keyboard::key(SDL_SCANCODE_KP_PLUS) != 0u) {
		scale += (0.005 * (Keyboard::key(SDL_SCANCODE_LSHIFT) + 1));
	}
	else if (Keyboard::key(SDL_SCANCODE_KP_MINUS) != 0u) {
		scale -= (0.005 * (Keyboard::key(SDL_SCANCODE_LSHIFT) + 1));
	}
	scale = CLAMP(scale, 0.0001, 1000);
	mesh->scale() = new_vec3(scale, scale, scale);
}

void	switch_background()
{
	static int	b = 0;
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

void	callback_background(SDL_KeyboardEvent *event)
{
	if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u))) {
		return ;
	}
	switch_background();
}

void	controller_callback_background(SDL_ControllerButtonEvent *event)
{
	if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN)) {
		return ;
	}
	switch_background();
}

void	controller_callback_quality(SDL_ControllerButtonEvent *event)
{
	if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN)) {
		return ;
	}
	GameController::get(0)->rumble(0.5, 100);
	Engine::internal_quality() += 0.25;
	Engine::internal_quality() = CYCLE(Engine::internal_quality(), 0.5, 1.5);
}

void	callback_quality(SDL_KeyboardEvent *event)
{
	if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u))) {
		return ;
	}
	Engine::internal_quality() += 0.25;
	Engine::internal_quality() = CYCLE(Engine::internal_quality(), 0.5, 1.5);
}

bool	rotate_model = true;

void	controller_callback_rotation(SDL_ControllerButtonEvent *event)
{
	if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN)) {
		return ;
	}
	rotate_model = !rotate_model;
}

void	callback_refresh(SDL_Event */*unused*/)
{
	static float	rotation = 0;
	auto	mesh = Engine::renderable(0);
	if (mesh == nullptr) {
		return ;
	}
	if (rotate_model)
	{
		rotation += 0.2 * Engine::delta_time();
		rotation = CYCLE(rotation, 0, 2 * M_PI);
		mesh->rotation() = new_vec3(0, rotation, 0);
	}
	callback_camera(nullptr);
}

void	callback_exit(SDL_KeyboardEvent */*unused*/)
{
	Engine::stop();
}

void	callback_fullscreen(SDL_KeyboardEvent *event)
{
	static bool	fullscreen = false;

	if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u)) {
		fullscreen = !fullscreen;
		Window::fullscreen(fullscreen);
	}
	(void)event;
}