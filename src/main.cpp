/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/06/03 18:36:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "Events.hpp"
#include "Window.hpp"
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

#include <iostream>

void	setup_callbacks()
{
	Events::set_key_callback(SDL_SCANCODE_KP_PLUS, callback_scale);
	Events::set_key_callback(SDL_SCANCODE_KP_MINUS, callback_scale);
	Events::set_key_callback(SDL_SCANCODE_SPACE, callback_background);
	Events::set_key_callback(SDL_SCANCODE_ESCAPE, callback_exit);
	Events::set_key_callback(SDL_SCANCODE_RETURN, callback_fullscreen);
	Events::set_key_callback(SDL_SCANCODE_Q, callback_quality);
	Events::set_refresh_callback(callback_refresh);
}

#include "parser/HDR.hpp"

int		main(int argc, char *argv[])
{
	Mesh *obj;
	auto argv0 = std::string(argv[0]);

	Window::init("Scop", WIDTH, HEIGHT);
	Engine::init(argv0);
	//light_create(new_vec3(-1, 1, 0), new_vec3(1, 1, 1), 1);
	HDR::parse("test", "./res/hdr/uffizi.hdr");
	auto camera = Camera::create("main_camera", 45);
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
	}
	Mesh::alpha_sort();
	setup_callbacks();
	Engine::run();
	SDL_Quit();
	return (0);
}
