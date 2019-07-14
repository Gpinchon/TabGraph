/*
* @Author: gpi
* @Date:   2019-03-27 13:38:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-07-13 14:32:16
*/

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "Camera.hpp" // for OrbitCamera, Camera
#include "Config.hpp" // for Config
#include "Engine.hpp" // for ResourcePath, Init, Start
#include "Light.hpp" // for DirectionnalLight
#include "Node.hpp" // for Node
#include "Terrain.hpp" // for Terrain
#include "scop.hpp" // for setup_callbacks
#include <SDL2/SDL.h> // for SDL_Quit
#include <SDL2/SDL_main.h> // for main
#include <glm/vec2.hpp> // for vec<>::vec<2, type-parameter-0-...
#include <glm/vec3.hpp> // for vec<>::vec<3, type-parameter-0-...
#include <math.h> // for M_PI
#include <memory> // for __shared_ptr_access, shared_ptr
#include <string> // for operator+, string

int main(int argc, char** argv)
{
    Config::Parse(Engine::ResourcePath() + "config.ini");
    Engine::Init();
    auto camera = OrbitCamera::create("main_camera", 45, M_PI / 2.f, M_PI / 2.f, 5.f);
    Camera::set_current(camera);
    camera->set_target(Node::create("main_camera_target", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
    camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);

    if (argc > 1)
        Terrain::create("terrain_test", glm::vec2(10, 10), argv[1]);
    else
        Terrain::create("terrain_test", glm::vec2(10, 10), Engine::ResourcePath() + "crater.bt");
    setup_callbacks();
    DirectionnalLight::create("MainLight", glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), 1, true);
    Engine::Start();
    SDL_Quit();
    return (0);
}