#include "Engine.hpp"
#include "Config.hpp"
#include "FPSCamera.hpp"
#include "Light.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Events.hpp"
#include "Scene.hpp"
#include "SceneParser.hpp"
#include "Window.hpp"
#include "Mesh.hpp"
#include "CubeMesh.hpp"
#include "parser/GLTF.hpp"

#define DOWNK SDL_SCANCODE_DOWN
#define UPK SDL_SCANCODE_UP
#define LEFTK SDL_SCANCODE_LEFT
#define RIGHTK SDL_SCANCODE_RIGHT
#define ZOOMK SDL_SCANCODE_KP_PLUS
#define UNZOOMK SDL_SCANCODE_KP_MINUS

void CameraCallback(SDL_Event *)
{
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    glm::vec2 raxis = glm::vec2(0, 0);
    glm::vec2 laxis = glm::vec2(0, 0);
    float taxis = 0;
    //Mouse::set_relative(SDL_TRUE);
    laxis.x = Keyboard::key(LEFTK) - Keyboard::key(RIGHTK);
    laxis.y = Keyboard::key(DOWNK) - Keyboard::key(UPK);
    raxis.x = 0;
    raxis.y = Keyboard::key(ZOOMK) - Keyboard::key(UNZOOMK);
    taxis += Keyboard::key(SDL_SCANCODE_PAGEUP);
    taxis -= Keyboard::key(SDL_SCANCODE_PAGEDOWN);
    camera->SetPosition(camera->Position() + float(Events::delta_time() * laxis.x * 100) * camera->Right());
    camera->SetPosition(camera->Position() - float(Events::delta_time() * laxis.y * 100) * camera->Forward());
    camera->SetPosition(camera->Position() + float(Events::delta_time() * taxis * 100) * Common::Up());
}

void MouseMoveCallback(SDL_MouseMotionEvent *event)
{
    if (!Mouse::button(1))
        return;
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    static glm::vec2 cameraRotation;
    cameraRotation.x += event->xrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
    cameraRotation.y -= event->yrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
    camera->SetYaw(cameraRotation.x);
    camera->SetPitch(cameraRotation.y);
}

void MouseWheelCallback(SDL_MouseWheelEvent *event)
{
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    camera->SetFov(camera->Fov() - event->y * 0.01);
    camera->SetFov(glm::clamp(camera->Fov(), 1.0f, 70.f));
}

void FullscreenCallback(SDL_KeyboardEvent* event) {
    if (Keyboard::key(SDL_SCANCODE_LALT) && !event->repeat) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::fullscreen(fullscreen);
    }
}

void ExitCallback(SDL_KeyboardEvent* ) {
    Engine::Stop();
}

void SetupCallbacks()
{
    Keyboard::set_callback(SDL_SCANCODE_ESCAPE, ExitCallback);
    Keyboard::set_callback(SDL_SCANCODE_RETURN, FullscreenCallback);
    //Mouse::set_relative(SDL_TRUE);
    Mouse::set_move_callback(MouseMoveCallback);
    Mouse::set_wheel_callback(MouseWheelCallback);
    Events::set_refresh_callback(CameraCallback);
}

int main(int argc, char **argv)
{
	if (argc <= 1)
		return -42;
    SetupCallbacks();
	Config::Parse(Engine::ResourcePath() + "config.ini");
    Engine::Init();
	auto scene(SceneParser::Parse(Engine::ExecutionPath() + std::string(argv[1])).at(0));
    if (scene == nullptr) {
        return -42;
    }
    //scene->Add(DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), 1, false));
    if (scene->CurrentCamera() == nullptr)
        scene->SetCurrentCamera(FPSCamera::Create("main_camera", 45));
    scene->CurrentCamera()->SetPosition(glm::vec3{10, 10, 0});
    scene->Add(CubeMesh::Create("cubeMesh", glm::vec3(100, 100 ,100)));
    Scene::SetCurrent(scene);
	Engine::Start();
	return 0;
}