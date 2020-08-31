/*
* @Author: gpinchon
* @Date:   2020-08-09 19:54:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 17:09:08
*/
#define USE_HIGH_PERFORMANCE_GPU
#include "Animation/Animation.hpp"
#include "Assets/AssetsParser.hpp"
#include "Callback.hpp"
#include "Camera/FPSCamera.hpp"
#include "Config.hpp"
#include "Engine.hpp"
#include "Input/Events.hpp"
#include "Input/Keyboard.hpp"
#include "Input/Mouse.hpp"
#include "Light/Light.hpp"
#include "Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Parser/GLTF.hpp"
#include "Render.hpp"
#include "Scene/Scene.hpp"
#include "Tools.hpp"
#include "Transform.hpp"
#include "Window.hpp"

#define DOWNK SDL_SCANCODE_DOWN
#define UPK SDL_SCANCODE_UP
#define LEFTK SDL_SCANCODE_LEFT
#define RIGHTK SDL_SCANCODE_RIGHT
#define ZOOMK SDL_SCANCODE_KP_PLUS
#define UNZOOMK SDL_SCANCODE_KP_MINUS

void CameraCallback()
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
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->Position() - float(Events::delta_time() * laxis.x * 100) * camera->GetComponent<Transform>()->Right());
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->Position() - float(Events::delta_time() * laxis.y * 100) * camera->GetComponent<Transform>()->Forward());
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->Position() + float(Events::delta_time() * taxis * 100) * Common::Up());
}

void MouseMoveCallback(SDL_MouseMotionEvent* event)
{
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    static glm::vec3 cameraRotation;
    if (Mouse::button(1)) {
        cameraRotation.x -= event->xrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
        cameraRotation.y -= event->yrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
    }
    if (Mouse::button(3))
        cameraRotation.z += event->xrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
    camera->SetYaw(cameraRotation.x);
    camera->SetPitch(cameraRotation.y);
    camera->SetRoll(cameraRotation.z);
}

void MouseWheelCallback(SDL_MouseWheelEvent* event)
{
    Scene::Current()->CurrentCamera()->SetFov(Scene::Current()->CurrentCamera()->Fov() - event->y);
    Scene::Current()->CurrentCamera()->SetFov(glm::clamp(Scene::Current()->CurrentCamera()->Fov(), 1.0f, 70.f));
}

void FullscreenCallback(const SDL_KeyboardEvent&)
{
    if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u)) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::fullscreen(fullscreen);
    }
}

void CallbackQuality(const SDL_KeyboardEvent& event)
{
    if (event.type == SDL_KEYUP || (event.repeat != 0u))
        return;
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

void CallbackAnimation(const SDL_KeyboardEvent& event)
{
    if (event.type == SDL_KEYUP || (event.repeat != 0u) || Scene::Current()->Animations().empty())
        return;
    static auto currentAnimation(0);
    Scene::Current()->Animations().at(currentAnimation)->Stop();
    currentAnimation++;
    currentAnimation %= Scene::Current()->Animations().size();
    Scene::Current()->Animations().at(currentAnimation)->SetRepeat(true);
    Scene::Current()->Animations().at(currentAnimation)->Play();
}

void ExitCallback(const SDL_KeyboardEvent&)
{
    Engine::Stop();
}

void ChangeCamera(const SDL_KeyboardEvent& event)
{
    if (event.type == SDL_KEYUP || !event.repeat)
        return;
    static auto currentCameraIndex(0u);
    auto& camera(Scene::Current()->Cameras().at(currentCameraIndex));
    currentCameraIndex++;
    currentCameraIndex %= Scene::Current()->Cameras().size();
    Scene::Current()->SetCurrentCamera(camera);
}

void SetupCallbacks()
{
    Keyboard::AddKeyCallback(SDL_SCANCODE_ESCAPE, Callback<void(const SDL_KeyboardEvent&)>::Create(ExitCallback, std::placeholders::_1));
    Keyboard::AddKeyCallback(SDL_SCANCODE_RETURN, Callback<void(const SDL_KeyboardEvent&)>::Create(FullscreenCallback, std::placeholders::_1));
    Keyboard::AddKeyCallback(SDL_SCANCODE_Q, Callback<void(const SDL_KeyboardEvent&)>::Create(CallbackQuality, std::placeholders::_1));
    Keyboard::AddKeyCallback(SDL_SCANCODE_A, Callback<void(const SDL_KeyboardEvent&)>::Create(CallbackAnimation, std::placeholders::_1));
    Keyboard::AddKeyCallback(SDL_SCANCODE_C, Callback<void(const SDL_KeyboardEvent&)>::Create(ChangeCamera, std::placeholders::_1));
    //Mouse::set_relative(SDL_TRUE);
    Mouse::set_move_callback(MouseMoveCallback);
    Mouse::set_wheel_callback(MouseWheelCallback);
    Events::AddRefreshCallback(Callback<void()>::Create(CameraCallback));
}

#include "StackTracer.hpp"
#include <csignal>
#include <filesystem>

int main(int argc, char** argv)
{
    if (argc <= 1)
        return -42;

    std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
    StackTracer::set_signal_handler(SIGABRT);
    Config::Parse(Engine::ResourcePath() / "config.ini");
    Engine::Init();
    std::filesystem::path filePath = (std::string(argv[1]));
    if (!filePath.is_absolute()) {
        filePath = Engine::ExecutionPath() / filePath;
    }
    {
        std::cout << filePath << std::endl;
        auto assets(AssetsParser::Parse(filePath.string()));
        auto scene = assets.GetComponent<Scene>();
        if (scene == nullptr) {
            return -43;
        }
        scene->SetCurrentCamera(FPSCamera::Create("main_camera", 45));
        scene->Add(DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(1000, 1000, 1000), 0.5, true));
        Scene::SetCurrent(scene);
    }
    SetupCallbacks();
    Engine::Start();
    return 0;
}