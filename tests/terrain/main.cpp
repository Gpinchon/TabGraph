/*
* @Author: gpi
* @Date:   2019-03-27 13:38:46
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 15:40:33
*/

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include <SDL2/SDL.h>       // for SDL_Quit
#include <SDL2/SDL_main.h>  // for main
#include <math.h>           // for M_PI
#include <memory>           // for __shared_ptr_access, shared_ptr, __shared...
#include <string>           // for operator+, string
#include "Camera.hpp"       // for OrbitCamera, Camera
#include "Config.hpp"       // for Config
//#include "DLLExport.hpp"  // lines 9-9
#include "Engine.hpp"       // for ResourcePath, Init, Start
#include "Light.hpp"        // for DirectionnalLight
#include "Node.hpp"         // for Node
#include "Terrain.hpp"      // for Terrain
#include "scop.hpp"         // for setup_callbacks
#include "vml.h"            // for new_vec3, new_vec2

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