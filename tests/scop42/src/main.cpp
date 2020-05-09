/*
* @Author: gpi
* @Date:   2019-03-26 13:04:12
* @Last Modified by:   gpi
* @Last Modified time: 2019-08-13 17:20:58
*/

#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "Config.hpp" // for Config
#include "BoundingElement.hpp"
#include "Engine.hpp" // for ProgramPath, Init, Start
#include "FPSCamera.hpp" // for FPSCamera
#include "Light.hpp" // for DirectionnalLight, Light
#include "Mesh.hpp" // for Mesh
#include "MeshParser.hpp" // for MeshParser
#include "Node.hpp" // for Node
#include "Scene.hpp"
#include "glm/glm.hpp" // for glm::vec3, glm::vec3
#include "scop.hpp" // for setup_callbacks
#include <SDL2/SDL.h> // for SDL_Quit
#include <SDL2/SDL_main.h> // for main
#include <cstdlib> // for rand, RAND_MAX
#include <math.h> // for M_PI
#include <memory> // for shared_ptr, __shared_ptr_access, operator!=
#include <string> // for operator+, to_string, string
#include <vector> // for vector

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

std::vector<std::shared_ptr<Light>> Create_random_lights(unsigned i)
{
    std::vector<std::shared_ptr<Light>> v;
    for (auto index = 0u; index < i; index++)
    {
        glm::vec3 Position = glm::vec3(
            (std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i,
            (std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i,
            (std::rand() / float(RAND_MAX) - 0.5) * 2.0 * i);
        glm::vec3 Color = glm::vec3(
            std::rand() / float(RAND_MAX),
            std::rand() / float(RAND_MAX),
            std::rand() / float(RAND_MAX));
        auto light = Light::Create("Light" + std::to_string(i), Color, Position, 1 / float(i));
        v.push_back(light);
    }
    return (v);
}

/*
auto	cube = CubeMesh::Create("cube", glm::vec3(1, 1, 1));
auto	compute_shader = GLSL::parse("CheckerBoard", "./compute/checkerboard.compute", ComputeShader);
auto	compute_object = ComputeObject::Create("computeObject", compute_shader);
auto	texture = Texture::Create("checkerBoardTexture", glm::vec2(256, 256), GL_TEXTURE_2D, GL_RGBA, GL_RGBA32F, GL_FLOAT);
//compute_object->set_in_texture(texture);
compute_object->set_out_texture(texture);
compute_object->Start();
cube->Geometry(0)->material()->set_texture_albedo(texture);
*/

#include <Geometry.hpp>

std::shared_ptr<Mesh> mainMesh = nullptr;

int main(int argc, char *argv[])
{
    Config::Parse(Engine::ProgramPath() + "./res/config.ini");
    Engine::Init();
    auto camera = FPSCamera::Create("main_camera", 45);
    Scene::SetCurrent(Scene::Create("Main Scene"));
    Scene::Current()->SetCurrentCamera(camera);
    //camera->SetTarget(Node::Create("main_camera_target", glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
    //camera->orbite(M_PI / 2.f, M_PI / 2.f, 5.f);
    if (argc >= 2)
    {
        mainMesh = MeshParser::parse("main_mesh", argv[1]);
    }
    if (argc < 2 || mainMesh == nullptr)
    {
        mainMesh = MeshParser::parse("main_mesh", Engine::ProgramPath() + "./res/obj/chart.obj");
    }
    /*if (mainMesh != nullptr)
    {
        for (auto &vg : mainMesh->Geometrys()) {
            for (auto &v : vg->v)
                v -= mainMesh->bounding_element->center;
        }
        mainMesh->bounding_element->min -= mainMesh->bounding_element->center;
        mainMesh->bounding_element->max -= mainMesh->bounding_element->center;
        mainMesh->bounding_element->center = glm::vec3(0, 0, 0);
    }*/
    Scene::Current()->Add(mainMesh);
    Scene::Current()->Add(camera);
    Scene::Current()->Add(DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), 1, true));
    setup_callbacks();
    //Create_random_lights(250);
    
    //DirectionnalLight::Create("BackLight", glm::vec3(0.3, 0.3, 0.3), glm::vec3(-10, -10, 0), 1, false);
    Engine::Start();
    SDL_Quit();
    return (0);
}