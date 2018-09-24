/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   callbacks.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/12 11:17:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 19:28:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "Mesh.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include "Keyboard.hpp"
#include "GameController.hpp"
#include "Environment.hpp"

static auto	cameraRotation = new_vec3(M_PI / 2.f, M_PI / 2.f, 5.f);

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
	//static VEC3	val = (VEC3){M_PI / 2.f, M_PI / 2.f, 5.f};
	cameraRotation.x += raxis.y * Engine::delta_time();
	cameraRotation.y += raxis.x * Engine::delta_time();
	cameraRotation.z -= laxis.y * Engine::delta_time();
	auto	camera = std::dynamic_pointer_cast<OrbitCamera>(Camera::current());
	auto	t = camera->target();
	t->position().y += rtrigger * Engine::delta_time();
	t->position().y -= ltrigger * Engine::delta_time();
	cameraRotation.x = CLAMP(cameraRotation.x, 0.01, M_PI - 0.01);
	cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
	cameraRotation.z = CLAMP(cameraRotation.z, 0.1f, 1000.f);
	camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
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
	mesh->scaling() = new_vec3(scale, scale, scale);
}

void	switch_background()
{
	static int	b = 0;
	b++;
	auto	env = Environment::get(b);
	if (env != nullptr) {
		Environment::set_current(env);
	}
	else {
		b = 0;
		env = Environment::get(b);
		Environment::set_current(env);
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

void	keyboard_callback_rotation(SDL_KeyboardEvent *event)
{
	if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u))) {
		return ;
	}
	rotate_model = !rotate_model;
}

void	callback_refresh(SDL_Event */*unused*/)
{
	static float	rotation = 0;
	auto	mesh = Renderable::get(0);
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

void	MouseWheelCallback(SDL_MouseWheelEvent *event)
{
	static auto	camera = std::dynamic_pointer_cast<OrbitCamera>(Camera::get_by_name("main_camera"));
	cameraRotation.z -= event->y;
	cameraRotation.x = CLAMP(cameraRotation.x, 0.01, M_PI - 0.01);
	cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
	cameraRotation.z = CLAMP(cameraRotation.z, 0.01, 1000.f);
	camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
}

void	MouseMoveCallback(SDL_MouseMotionEvent *event)
{
	static auto	camera = std::dynamic_pointer_cast<OrbitCamera>(Camera::get_by_name("main_camera"));
	/*if (Mouse::button(2))
	{
		auto	world_mouse_pos = mat4_mult_vec4(camera->projection(), new_vec4(event->xrel, event->yrel, 0, 1));
		auto	world_mouse_pos3 = new_vec3(world_mouse_pos.x, world_mouse_pos.y, world_mouse_pos.z);
		world_mouse_pos3 = vec3_normalize(vec3_sub(camera->position(), world_mouse_pos3));
		camera->target()->position().x += world_mouse_pos3.x;
		camera->target()->position().y += world_mouse_pos3.y;
		camera->target()->position().z += world_mouse_pos3.z;
		std::cout << world_mouse_pos.x << " " << world_mouse_pos.y << " " << world_mouse_pos.z << std::endl;
	}*/
	if (Mouse::button(1)) {
		cameraRotation.x += event->yrel * Engine::delta_time();
		cameraRotation.y -= event->xrel * Engine::delta_time();
		cameraRotation.x = CLAMP(cameraRotation.x, 0.01, M_PI - 0.01);
		cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
		cameraRotation.z = CLAMP(cameraRotation.z, 0.01, 1000.f);
	}
	camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
}