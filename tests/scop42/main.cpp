/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:09 by gpinchon          #+#    #+#             */
/*   Updated: 2019/03/03 16:46:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "tests/scop.hpp"
#include "Config.hpp"
#include "Window.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "Events.hpp"
#include "GameController.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "parser/OBJ.hpp"
#include "parser/HDR.hpp"
#include "parser/FBX.hpp"
#include "parser/GLSL.hpp"
#include "Material.hpp"
#include "Render.hpp"
#include "Light.hpp"
#include "CubeMesh.hpp"
#include "Debug.hpp"
#include "ComputeObject.hpp"
#include "Vgroup.hpp"
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <csignal>

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

/*
auto	cube = CubeMesh::create("cube", new_vec3(1, 1, 1));
auto	compute_shader = GLSL::parse("CheckerBoard", "./compute/checkerboard.compute", ComputeShader);
auto	compute_object = ComputeObject::create("computeObject", compute_shader);
auto	texture = Texture::create("checkerBoardTexture", new_vec2(256, 256), GL_TEXTURE_2D, GL_RGBA, GL_RGBA32F, GL_FLOAT);
//compute_object->set_in_texture(texture);
compute_object->set_out_texture(texture);
compute_object->run();
cube->vgroup(0)->material()->set_texture_albedo(texture);
*/

int		main(int argc, char *argv[])
{
	std::shared_ptr<Mesh>	obj;
	auto argv0 = std::string(argv[0]);

	Config::Load(Engine::resource_path() + "config.ini");
	Engine::init();
	auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
	Camera::set_current(camera);
	camera->set_target(Node::create("main_camera_target", new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
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
	//FBX::parseBin(Engine::program_path() + "./mug.fbx");
	setup_callbacks();
	//create_random_lights(250);
	DirectionnalLight::create("MainLight", new_vec3(1, 1, 1), new_vec3(10, 10, 10), 1, true);
	//DirectionnalLight::create("BackLight", new_vec3(0.3, 0.3, 0.3), new_vec3(-10, -10, 0), 1, false);
	Engine::run();
	SDL_Quit();
	return (0);
}