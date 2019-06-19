/*
* @Author: gpi
* @Date:   2019-03-27 13:38:46
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-19 13:46:15
*/

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "scop.hpp"
#include "Config.hpp"
#include "Terrain.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <csignal>

int		main(int argc, char **argv)
{
	Config::Load(Engine::ResourcePath() + "config.ini");
	Engine::Init();
	auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
	Camera::set_current(camera);
	camera->set_target(Node::create("main_camera_target", new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);

	if (argc > 1)
		Terrain::create("terrain_test", new_vec2(1024, 1024), argv[1]);
	else
		Terrain::create("terrain_test", new_vec2(1024, 1024), Engine::ResourcePath() + "crater.bt");
	setup_callbacks();
	DirectionnalLight::create("MainLight", new_vec3(1, 1, 1), new_vec3(10, 10, 10), 1, true);
	Engine::Start();
	SDL_Quit();
	return (0);
}