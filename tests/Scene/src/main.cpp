#define USE_HIGH_PERFORMANCE_GPU
#include "Animation/Animation.hpp"
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
#include "Scene/SceneParser.hpp"
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

void FullscreenCallback(SDL_KeyboardEvent*)
{
    if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u)) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::fullscreen(fullscreen);
    }
}

void CallbackQuality(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
        return;
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

void CallbackAnimation(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)) || Scene::Current()->Animations().empty())
        return;
    static auto currentAnimation(0);
    Scene::Current()->Animations().at(currentAnimation)->Stop();
    currentAnimation++;
    currentAnimation %= Scene::Current()->Animations().size();
    Scene::Current()->Animations().at(currentAnimation)->SetRepeat(true);
    Scene::Current()->Animations().at(currentAnimation)->Play();
}

void ExitCallback(SDL_KeyboardEvent*)
{
    Engine::Stop();
}

void ChangeCamera(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
        return;
    static auto currentCameraIndex(0u);
    auto& camera(Scene::Current()->Cameras().at(currentCameraIndex));
    currentCameraIndex++;
    currentCameraIndex %= Scene::Current()->Cameras().size();
    Scene::Current()->SetCurrentCamera(camera);
}

void SetupCallbacks()
{
    Keyboard::set_callback(SDL_SCANCODE_ESCAPE, ExitCallback);
    Keyboard::set_callback(SDL_SCANCODE_RETURN, FullscreenCallback);
    Keyboard::set_callback(SDL_SCANCODE_Q, CallbackQuality);
    Keyboard::set_callback(SDL_SCANCODE_A, CallbackAnimation);
    Keyboard::set_callback(SDL_SCANCODE_C, ChangeCamera);
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
    Config::Parse(Engine::ResourcePath() + "config.ini");
    Engine::Init();
    std::filesystem::path filePath = (std::string(argv[1]));
    if (!filePath.is_absolute()) {
        filePath = Engine::ExecutionPath() / filePath;
    }
    auto scene(SceneParser::Parse(filePath.string()).at(0));
    if (scene == nullptr) {
        return -42;
    }
    scene->SetCurrentCamera(FPSCamera::Create("main_camera", 45));
    scene->Add(DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(1000, 1000, 1000), 0.5, true));

    /*
    auto scene(Scene::Create("MainScene"));
    scene->SetCurrentCamera(FPSCamera::Create("main_camera", 45));
    scene->CurrentCamera()->GetComponent<Transform>()->SetPosition(glm::vec3{0, 0, 0});
    auto cube0(CubeMesh::Create("Cube0", glm::vec3(1, 1, 1)));
    auto cube1(CubeMesh::Create("Cube1", glm::vec3(1, 1, 1)));
    auto cube2(CubeMesh::Create("Cube2", glm::vec3(1, 1, 1)));
    cube0->GetMaterial(0)->SetAlbedo(glm::vec3(1, 0, 0));
    cube1->GetMaterial(0)->SetAlbedo(glm::vec3(0, 1, 0));
    cube2->GetMaterial(0)->SetAlbedo(glm::vec3(0, 0, 1));
    cube1->SetParent(cube0);
    cube2->SetParent(cube1);
    cube0->GetComponent<Transform>()->SetLocalTransform(
        glm::mat4x4(glm::vec4(0.388609, 0.000000, 0.921403, 0.000000),
                    glm::vec4(-0.300040, 0.945496, 0.126544, 0.000000),
                    glm::vec4(-0.871183, -0.325634, 0.367428, 0.000000),
                    glm::vec4(-2.968655, -0.502453, 1.696065, 1.000000)));
    cube1->GetComponent<Transform>()->SetPosition(glm::vec3(0, 1, 0));
    cube2->GetComponent<Transform>()->SetPosition(glm::vec3(0, 1, 0));
    cube1->GetComponent<Transform>()->SetRotation(glm::vec3(1, 0, 0));
    scene->Add(cube0);
*/

    Scene::SetCurrent(scene);
    SetupCallbacks();
    Engine::Start();
    return 0;
}