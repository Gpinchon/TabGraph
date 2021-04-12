/*
* @Author: gpinchon
* @Date:   2020-08-09 19:54:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 17:09:08
*/
#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "Animation/Animation.hpp"
#include "Assets/Asset.hpp"
#include "Assets/AssetsParser.hpp"
#include "Camera/FPSCamera.hpp"
#include "Config.hpp"
#include "Engine.hpp"
#include "Event/Events.hpp"
#include "Event/Keyboard.hpp"
#include "Event/Mouse.hpp"
#include "Light/DirectionalLight.hpp"
#include "Material/Material.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Scene.hpp"
#include "Tools/Tools.hpp"
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

void CameraCallback(float delta)
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
    camera->SetPosition(camera->GetPosition() - float(delta * laxis.x * speed) * camera->Right());
    camera->SetPosition(camera->GetPosition() - float(delta * laxis.y * speed) * camera->Forward());
    camera->SetPosition(camera->GetPosition() + float(delta * taxis * speed) * Common::Up());
}

void MouseMoveCallback(const SDL_MouseMotionEvent& event)
{
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    static glm::vec3 cameraRotation;
    if (Mouse::button(1)) {
        cameraRotation.x -= event.xrel * 0.05 * Config::Global().Get("MouseSensitivity", 2.f);
        cameraRotation.y -= event.yrel * 0.05 * Config::Global().Get("MouseSensitivity", 2.f);
    }
    if (Mouse::button(3))
        cameraRotation.z += event.xrel * 0.05 * Config::Global().Get("MouseSensitivity", 2.f);
    camera->SetYaw(cameraRotation.x);
    camera->SetPitch(cameraRotation.y);
    camera->SetRoll(cameraRotation.z);
}

void MouseWheelCallback(const SDL_MouseWheelEvent& event)
{
    Scene::Current()->CurrentCamera()->SetFov(Scene::Current()->CurrentCamera()->Fov() - event.y);
    Scene::Current()->CurrentCamera()->SetFov(glm::clamp(Scene::Current()->CurrentCamera()->Fov(), 1.0f, 70.f));
}

void FullscreenCallback(const SDL_KeyboardEvent&)
{
    if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u)) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::SetFullscreen(fullscreen);
    }
}

void CallbackQuality(const SDL_KeyboardEvent& event)
{
    if (event.type == SDL_KEYUP || (event.repeat != 0u))
        return;
    auto quality = CYCLE(Config::Global().Get("InternalQuality", 1.f) + 0.25f, 0.5f, 1.5f);
    Config::Global().Set("InternalQuality", quality);
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

void ControllerButton(const SDL_ControllerButtonEvent& event) {
    std::cout << (event.type == SDL_CONTROLLERBUTTONDOWN ? "Button pressed " : "Button released ") << event.button << " on Controller " << event.which << std::endl;
}

#include "Event/GameController.hpp"

void SetupCallbacks()
{
    GameController::Get(0)->OnButton(SDL_CONTROLLER_BUTTON_A).Connect(&ControllerButton);
    Keyboard::OnKey(SDL_SCANCODE_KP_PLUS).Connect(CallbackSpeed);
    Keyboard::OnKey(SDL_SCANCODE_KP_MINUS).Connect(CallbackSpeed);
    Keyboard::OnKey(SDL_SCANCODE_ESCAPE).Connect(ExitCallback);
    Keyboard::OnKey(SDL_SCANCODE_RETURN).Connect(FullscreenCallback);
    Keyboard::OnKey(SDL_SCANCODE_Q).Connect(CallbackQuality);
    Keyboard::OnKey(SDL_SCANCODE_A).Connect(CallbackAnimation);
    Keyboard::OnKey(SDL_SCANCODE_C).Connect(ChangeCamera);
    //Mouse::set_relative(SDL_TRUE);
    Mouse::OnMove().Connect(&MouseMoveCallback);
    Mouse::OnWheel().Connect(&MouseWheelCallback);
    Events::OnRefresh().Connect(CameraCallback);
    //Events::AddRefreshCallback(Callback<void()>::Create(CameraCallback));
}

#include "StackTracer.hpp"
#include <csignal>
#include <filesystem>
#include "Texture/Cubemap.hpp"
#include "Skybox.hpp"
#include "Assets/Image.hpp"
#include "Light/PointLight.hpp"
#include "Light/SkyLight.hpp"
#include "Light/HDRLight.hpp"

int main(int argc, char** argv)
{
    if (argc <= 1)
        return -42;
    {
        //std::set_terminate([]() { std::cout << "Unhandled exception\n"; int* p = nullptr; p[0] = 1; });
        StackTracer::set_signal_handler(SIGABRT);
        Config::Global().Parse(Engine::ResourcePath() / "config.ini");
        Engine::Init();
        std::filesystem::path filePath = (std::string(argv[1]));
        if (!filePath.is_absolute()) {
            filePath = Engine::ExecutionPath() / filePath;
        }
        {
            std::cout << filePath << std::endl;
            auto fpsCamera = Component::Create<FPSCamera>("main_camera", 45);
            //fpsCamera->SetZfar(1000);
            auto asset{ Component::Create<Asset>("file:" + filePath.string()) };
            asset->Load();
            auto assetCameras = asset->GetComponents<Camera>();
            s_cameras.push_back(fpsCamera);
            s_cameras.insert(s_cameras.end(), assetCameras.begin(), assetCameras.end());
            auto scene = std::static_pointer_cast<Scene>(asset->GetComponent<Scene>());
            if (scene == nullptr) {
                return -43;
            }
            scene->SetCurrentCamera(fpsCamera);
            auto newEnv = Component::Create<Skybox>("Skybox");
            auto diffuseAsset{ Component::Create<Asset>("file:" + (Engine::ResourcePath() / "env/diffuse.hdr").string()) };
            newEnv->SetTexture(Component::Create<Cubemap>(diffuseAsset));//"EnvironmentCube", TextureParser::parse("Skybox", (Engine::ResourcePath() / "env/diffuse.hdr").string())));
            scene->SetSkybox(newEnv);

            auto hdrLight{ Component::Create<HDRLight>(diffuseAsset) };
            //auto skyLight{ Component::Create<SkyLight>() };
            //skyLight->SetSunDirection(glm::vec3(1, 1, 1));
            //skyLight->SetSpecularPower(0);
            //auto dirLight =  Component::Create<DirectionnalLight>("MainLight", glm::vec3(0.025), -skyLight->GetSunDirection(), false);
            //dirLight->SetHalfSize(glm::vec3(50));
            auto dirLight = Component::Create<DirectionalLight>("MainLight", glm::vec3(0.5), glm::vec3(1, 1, 1), false);
            auto pointLight = Component::Create<PointLight>("PointLight", glm::vec3(1, 1, 1));
            //pointLight->SetPosition(glm::vec3(0, 0.1, 0));
            pointLight->SetPower(2);
            pointLight->SetParent(fpsCamera);
            s_light = pointLight;
            
            scene->Add(hdrLight);
            //scene->Add(skyLight);
            scene->Add(dirLight);
            scene->Add(pointLight);
            
            Scene::SetCurrent(scene);
        }
        SetupCallbacks();
        Engine::Start();
    }
    return 0;
}