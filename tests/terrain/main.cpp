/*
* @Author: gpi
* @Date:   2019-03-27 13:38:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-05-08 13:04:33
*/

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "scop.hpp"
#include "Config.hpp"
#include "Window.hpp"
#include "Engine.hpp"
#include "Camera.hpp"
#include "Events.hpp"
#include "GameController.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "TextureParser.hpp"
#include "parser/OBJ.hpp"
#include "parser/FBX.hpp"
#include "parser/GLSL.hpp"
#include "parser/TerrainData.hpp"
#include "Material.hpp"
#include "Render.hpp"
#include "Light.hpp"
#include "CubeMesh.hpp"
#include "Debug.hpp"
#include "ComputeObject.hpp"
#include "Vgroup.hpp"
#include "Terrain.hpp"
#include <iostream>
#include <unistd.h>
#include <iostream>
#include <csignal>

int		main(int /*argc*/, char **/*argv*/)
{
	Config::Load(Engine::resource_path() + "config.ini");
	Engine::init();
	auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
	Camera::set_current(camera);
	camera->set_target(Node::create("main_camera_target", new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1)));
	camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);

	Terrain::create("terrain_test", new_vec2(1024, 1024), Engine::resource_path() + "crater.bt");
	//FBX::parseBin(Engine::program_path() + "./mug.fbx");
	setup_callbacks();
	//create_random_lights(250);
	DirectionnalLight::create("MainLight", new_vec3(1, 1, 1), new_vec3(10, 10, 10), 1, true);
	//DirectionnalLight::create("BackLight", new_vec3(0.3, 0.3, 0.3), new_vec3(-10, -10, 0), 1, false);
	Engine::run();
	SDL_Quit();
	return (0);
}