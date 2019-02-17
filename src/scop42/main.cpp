/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/15 23:41:28 by gpinchon         ###   ########.fr       */
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

/*bool	alpha_compare(Renderable	*m, Renderable *m1)
{
	auto	mat = m->material;
	auto	mat1 = m1->material;
	if (m->parent == m1) {
		return (false);
	} if (m1->parent == m) {
		return (true);
	}
	if ((mat == nullptr) && (mat1 == nullptr)) {
		return (false);
	}
	if (mat == nullptr) {
		return (true);
	}
	if (mat1 == nullptr) {
		return (false);
	}
	return mat->alpha > mat1->alpha || ((mat->texture_albedo != nullptr) && (mat1->texture_albedo != nullptr) &&
		mat->texture_albedo->bpp() <= 24 && mat1->texture_albedo->bpp() >= 32);
}*/

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
	Events::set_refresh_callback(callback_refresh);
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

#include "Debug.hpp"
#include "ComputeObject.hpp"
#include "Vgroup.hpp"

int		main(int argc, char *argv[])
{
	std::shared_ptr<Mesh>	obj;
	auto argv0 = std::string(argv[0]);

	Engine::init();
	auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
	Camera::set_current(camera);
	camera->set_target(Node::create("main_camera_target", new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);
	/*auto	cube = CubeMesh::create("cube", new_vec3(1, 1, 1));
	auto	compute_shader = GLSL::parse("CheckerBoard", "./compute/checkerboard.compute", ComputeShader);
	auto	compute_object = ComputeObject::create("computeObject", compute_shader);
	auto	texture = Texture::create("checkerBoardTexture", new_vec2(256, 256), GL_TEXTURE_2D, GL_RGBA, GL_RGBA32F, GL_FLOAT);
	//compute_object->set_in_texture(texture);
	compute_object->set_out_texture(texture);
	compute_object->run();
	cube->vgroup(0)->material()->set_texture_albedo(texture);*/
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
	//FBX::parseBin(Engine::program_path() + "./mug.fbx");
	setup_callbacks();
	//create_random_lights(250);
	DirectionnalLight::create("MainLight", new_vec3(1, 1, 1), new_vec3(10, 10, 10), 1, true);
	//DirectionnalLight::create("BackLight", new_vec3(0.3, 0.3, 0.3), new_vec3(-10, -10, 0), 1, false);
	Engine::run();
	SDL_Quit();
	return (0);
}