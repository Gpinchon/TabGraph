/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/17 11:09:26 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Window.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "Events.hpp"
#include "GameController.hpp"
#include "Keyboard.hpp"
#include "parser/OBJ.hpp"
#include <unistd.h>

/*int		light_create(VEC3 position, VEC3 color, float power, bool cast_shadow = false)
{
	t_light *l;
	GLenum	e;

	(void)e;
	l = new t_light();
	if (cast_shadow)
	{
		l->render_buffer = framebuffer_create(new_vec2(SHADOWRES, SHADOWRES),
		shader_get_by_name("shadow"), 0, 1);
		texture_set_parameter(framebuffer_get_depth(l->render_buffer), GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		texture_set_parameter(framebuffer_get_depth(l->render_buffer), GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		l->data.directional.power = power;
		l->data.directional.color = color;
		l->transform_index = transform_create(position, new_vec3(0, 0, 0),
		new_vec3(1, 1, 1));
	}
	Engine::get().lights.push_back(l);
	return (Engine::get().lights.size() - 1);
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
#include <iostream>

std::vector<Light *> create_random_lights(unsigned i)
{
	std::vector<Light *> v;
	for (auto index = 0u; index < i; index++)
	{
		Light *light;
		VEC3	Position = new_vec3(
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i,
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i,
			(std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i);
		VEC3	Color = new_vec3(
			std::rand() / float(RAND_MAX),
			std::rand() / float(RAND_MAX),
			std::rand() / float(RAND_MAX));
		light = Light::create("Light" + std::to_string(i), Color, Position, 1 / float(i));
		v.push_back(light);
	}
	return (v);
}

int		main(int argc, char *argv[])
{
	Mesh *obj;
	auto argv0 = std::string(argv[0]);

	//Window::init("Scop", WIDTH, HEIGHT);
	Engine::init();
	//light_create(new_vec3(-1, 1, 0), new_vec3(1, 1, 1), 1);
	auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
	Engine::set_current_camera(camera);
	camera->target = Node::create("main_camera_target",
		new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);
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
		//obj->sort(alpha_compare);
	}
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