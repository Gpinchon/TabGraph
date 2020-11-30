/*
* @Author: gpinchon
* @Date:   2020-08-09 19:54:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 17:09:08
*/
//#define USE_HIGH_PERFORMANCE_GPU
//#include "DLLExport.hpp"

#include "Animation/Animation.hpp"
#include "Assets/AssetsParser.hpp"
#include "Callback.hpp"
#include "Camera/FPSCamera.hpp"
#include "Config.hpp"
#include "Engine.hpp"
#include "Event/Events.hpp"
#include "Event/Keyboard.hpp"
#include "Event/Mouse.hpp"
#include "Light/DirectionnalLight.hpp"
#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Parser/GLTF.hpp"
#include "Render.hpp"
#include "Scene/Scene.hpp"
#include "Tools/Tools.hpp"
#include "Transform.hpp"
#include "Window.hpp"

#define DOWNK SDL_SCANCODE_DOWN
#define UPK SDL_SCANCODE_UP
#define LEFTK SDL_SCANCODE_LEFT
#define RIGHTK SDL_SCANCODE_RIGHT
#define ZOOMK SDL_SCANCODE_KP_PLUS
#define UNZOOMK SDL_SCANCODE_KP_MINUS

float speed = 1.f;

void CallbackSpeed(const SDL_KeyboardEvent& event) {
    if (event.type == SDL_KEYUP)
        return;
    if (Keyboard::key(SDL_SCANCODE_KP_MINUS) != 0u) {
        --speed;
    }
    else if (Keyboard::key(SDL_SCANCODE_KP_PLUS) != 0u) {
        ++speed;
    }
    speed = std::max(1.f, speed);
}

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
    camera->SetPosition(camera->GetPosition() - float(Events::delta_time() * laxis.x * speed) * camera->Right());
    camera->SetPosition(camera->GetPosition() - float(Events::delta_time() * laxis.y * speed) * camera->Forward());
    camera->SetPosition(camera->GetPosition() + float(Events::delta_time() * taxis * speed) * Common::Up());
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
    if (event.type == SDL_KEYUP || (event.repeat != 0u) || Scene::Current()->GetComponents<Animation>().empty())
        return;
    static auto animations = Scene::Current()->GetComponents<Animation>();
    static auto currentAnimation(animations.begin());
    (*currentAnimation)->Stop();
    currentAnimation++;
    if (currentAnimation == animations.end())
        currentAnimation = animations.begin();
    //currentAnimation %= Scene::Current()->Animations().size();
    (*currentAnimation)->SetRepeat(true);
    (*currentAnimation)->Play();
}

void ExitCallback(const SDL_KeyboardEvent&)
{
    Engine::Stop();
}

std::shared_ptr<Light> s_light;
std::vector<std::shared_ptr<Camera>> s_cameras;

void ChangeCamera(const SDL_KeyboardEvent& event)
{
    if (event.type == SDL_KEYUP || (event.repeat != 0u))
        return;
    static auto s_currentCamera = 0u;
    s_currentCamera++;
    if (s_currentCamera >= s_cameras.size())
        s_currentCamera = 0;
    Scene::Current()->SetCurrentCamera(s_cameras.at(s_currentCamera));
    s_light->SetParent(Scene::Current()->CurrentCamera());
}

void SetupCallbacks()
{
    Keyboard::AddKeyCallback(SDL_SCANCODE_KP_PLUS, Callback<void(const SDL_KeyboardEvent&)>::Create(CallbackSpeed, std::placeholders::_1));
    Keyboard::AddKeyCallback(SDL_SCANCODE_KP_MINUS, Callback<void(const SDL_KeyboardEvent&)>::Create(CallbackSpeed, std::placeholders::_1));
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
#include "Texture/Cubemap.hpp"
#include "Environment.hpp"
#include "Texture/TextureParser.hpp"

int main(int argc, char** argv)
{
    if (argc <= 1)
        return -42;
    {
        std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
        StackTracer::set_signal_handler(SIGABRT);
        Config::Parse(Engine::ResourcePath() / "config.ini");
        Engine::Init();
        std::filesystem::path filePath = (std::string(argv[1]));
        if (!filePath.is_absolute()) {
            filePath = Engine::ExecutionPath() / filePath;
        }
        //{
            std::cout << filePath << std::endl;
            auto fpsCamera = Component::Create<FPSCamera>("main_camera", 45);
            auto assets(AssetsParser::Parse(filePath.string()));
            auto assetCameras = assets->GetComponents<Camera>();
            s_cameras.push_back(fpsCamera);
            s_cameras.insert(s_cameras.end(), assetCameras.begin(), assetCameras.end());
            auto scene = std::static_pointer_cast<Scene>(assets->GetComponent<Scene>());
            if (scene == nullptr) {
                return -43;
            }
            scene->SetCurrentCamera(fpsCamera);
            auto dirLight =  Component::Create<DirectionnalLight>("MainLight", glm::vec3(1), glm::vec3(1, 10, 1), true);
            dirLight->SetHalfSize(glm::vec3(50));
            s_light = dirLight;
            s_light->SetParent(scene->CurrentCamera());
            scene->Add(s_light);
            auto newEnv = Component::Create<Environment>("Environment");
            newEnv->set_diffuse(Component::Create<Cubemap>("EnvironmentCube", TextureParser::parse("Environment", (Engine::ResourcePath() / "env/diffuse.hdr").string())));
            newEnv->set_irradiance(Component::Create<Cubemap>("EnvironmentDiffuse", TextureParser::parse("EnvironmentDiffuse", (Engine::ResourcePath() / "env/environment.hdr").string())));
            scene->SetEnvironment(newEnv);
            Scene::SetCurrent(scene);
        //}
        SetupCallbacks();
        Engine::Start();
    }
    _CrtDumpMemoryLeaks();
    return 0;
}