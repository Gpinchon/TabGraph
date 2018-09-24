/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 12:04:29 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Window.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "Events.hpp"
#include "GameController.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "parser/OBJ.hpp"
#include <unistd.h>
#include <iostream>

static auto	cameraRotation = new_vec3(M_PI / 2.f, M_PI / 2.f, 5.f);

void	MouseWheelCallback(SDL_MouseWheelEvent *event)
{
	static auto	camera = std::dynamic_pointer_cast<OrbitCamera>(Camera::get_by_name("main_camera"));
	cameraRotation.z -= event->y;
	cameraRotation.x = CLAMP(cameraRotation.x, 0.01, M_PI - 0.01);
	cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
	cameraRotation.z = CLAMP(cameraRotation.z, 0, 1000.f);
	camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
}

void	MouseMoveCallback(SDL_MouseMotionEvent *event)
{
	if (!Mouse::button(1))
		return ;
	static auto	camera = std::dynamic_pointer_cast<OrbitCamera>(Camera::get_by_name("main_camera"));
	cameraRotation.x += event->yrel * Engine::delta_time();
	cameraRotation.y -= event->xrel * Engine::delta_time();
	cameraRotation.x = CLAMP(cameraRotation.x, 0.01, M_PI - 0.01);
	cameraRotation.y = CYCLE(cameraRotation.y, 0, 2 * M_PI);
	cameraRotation.z = CLAMP(cameraRotation.z, 0.01, 1000.f);
	camera->orbite(cameraRotation.x, cameraRotation.y, cameraRotation.z);
	/*auto	mesh = Engine::renderable(0);
	auto	rotation = mesh->rotation();
	rotation.x += event->yrel * 0.01;
	rotation.y += event->xrel * 0.01;
	rotation.x = CYCLE(rotation.x, 0, 2 * M_PI);
	rotation.y = CYCLE(rotation.y, 0, 2 * M_PI);
	mesh->rotation() = rotation;*/
	//std::cout << "X : " << event->xrel << " Y : " << event->yrel << std::endl;
}

void	setup_callbacks()
{
	Keyboard::set_callback(SDL_SCANCODE_KP_PLUS, callback_scale);
	Keyboard::set_callback(SDL_SCANCODE_KP_MINUS, callback_scale);
	Keyboard::set_callback(SDL_SCANCODE_SPACE, callback_background);
	Keyboard::set_callback(SDL_SCANCODE_ESCAPE, callback_exit);
	Keyboard::set_callback(SDL_SCANCODE_RETURN, callback_fullscreen);
	Keyboard::set_callback(SDL_SCANCODE_Q, callback_quality);
	Keyboard::set_callback(SDL_SCANCODE_R, keyboard_callback_rotation);
	Mouse::set_relative(SDL_TRUE);
	Mouse::set_move_callback(MouseMoveCallback);
	Mouse::set_wheel_callback(MouseWheelCallback);
	//Events::set_refresh_callback(callback_refresh);
	auto controller = GameController::get(0);
	if (controller == nullptr)
		return ;
	controller->set_button_callback(SDL_CONTROLLER_BUTTON_A, controller_callback_quality);
	controller->set_button_callback(SDL_CONTROLLER_BUTTON_B, controller_callback_rotation);
	controller->set_button_callback(SDL_CONTROLLER_BUTTON_Y, controller_callback_background);
	
}

#include "parser/HDR.hpp"
#include "parser/FBX.hpp"
#include "parser/GLSL.hpp"
#include "Material.hpp"
#include "Render.hpp"
#include "Light.hpp"
#include "CubeMesh.hpp"
#include <iostream>

std::vector<std::shared_ptr<Light>> create_random_lights(unsigned i)
{
	std::vector<std::shared_ptr<Light>> v;
	for (auto index = 0u; index < i; index++)
	{
		VEC3	Position = new_vec3(
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i,
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i,
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i);
		VEC3	Color = new_vec3(
			std::rand() / float(RAND_MAX),
			std::rand() / float(RAND_MAX),
			std::rand() / float(RAND_MAX));
		auto	light = Light::create("Light" + std::to_string(i), Color, Position, 1 / float(i));
		v.push_back(light);
	}
	return (v);
}

#include "Errors.hpp"

int		main(int argc, char *argv[])
{
	std::shared_ptr<Mesh>	obj;
	auto argv0 = std::string(argv[0]);

	Engine::init();
	auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
	Camera::set_current(camera);
	camera->set_target(Node::create("main_camera_target", new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);
	/*auto cube0 = CubeMesh::create("cube0", new_vec3(1, 2, 1));
	auto cube1 = CubeMesh::create("cube1", new_vec3(1, 1, 1));
	cube1->position() = new_vec3(0, 1.5, 0);
	cube1->rotation() = new_vec3(0, 0, 0.5);
	cube1->parent = cube0;*/
	obj = nullptr;
	if (argc >= 2) {
		obj = OBJ::parse("main_mesh", argv[1]);
	}
	if (argc > 2 || obj == nullptr) {
		obj = OBJ::parse("main_mesh", Engine::program_path() + "./res/obj/chart.obj");
	}
	if (obj != nullptr) {
		obj->center();
		obj->load();
	}
	//obj->parent = cube1;
	//FBX::parseBin(Engine::program_path() + "./mug.fbx");
	Render::add_post_treatment("SSAO", Engine::program_path() + "./res/shaders/ssao.frag");
	setup_callbacks();
	//create_random_lights(250);
	DirectionnalLight::create("MainLight", new_vec3(1, 1, 1), new_vec3(10, 10, 0), 1, true);
	DirectionnalLight::create("BackLight", new_vec3(0.3, 0.3, 0.3), new_vec3(-10, -10, 0), 1, false);
	Engine::run();
	SDL_Quit();
	return (0);
}