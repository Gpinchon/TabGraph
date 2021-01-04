/*
* @Author: gpinchon
* @Date:   2020-08-09 19:54:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-20 17:09:08
*/
#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

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
    auto quality = CYCLE(Config::Get("InternalQuality", 1.f) + 0.25f, 0.5f, 1.5f);
    Config::Set("InternalQuality", quality);
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

//std::shared_ptr<Light> s_light;
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
    //s_light->SetParent(Scene::Current()->CurrentCamera());
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
#include "Light/PointLight.hpp"

auto Factorial(int n)
{
    if (n == 0)
        return 1;
    return n * Factorial(n - 1);
}

template <typename T>
T BinomialCoefficient(T n, T k) {
    if (k > n)
        return T(0);
    if (k == 0 || k == n)
        return T(1);
    return BinomialCoefficient(n - 1, k - 1) + BinomialCoefficient(n - 1, k);
    //return n / k * BinomialCoefficient(n - 1, k - 1);
    //return Factorial(n) / (Factorial(k) * Factorial(n - k));
}

int RodriguesDegree(int n)
{
    return n > 2 ? n : 2;
}

template <typename T>
T Derivate(T x, T n) {
    if (n == 0)
        return T(1);
    return n * Derivate(x, n - 1);
}

float LegendrePolynomial(int n, float x)
{
    if (n == 0)
        return 1;
    else if (n == 1)
        return x;
    float d = RodriguesDegree(n);
    float a = 1 / (pow(2, n) * Factorial(n));
    float b = pow(d, n) / (d * pow(x, n));
    float c = pow(x, 2) - 1;
    return a * b * c;
}

auto LegendreIntegralFactor(int m, int l, float x, int k) {
    if (k > l)
        return 0.0;
    auto a = Factorial(k) / Factorial(k - m);
    auto b = pow(x, k - m);
    auto lk = (l + k) / 2;
    auto c = BinomialCoefficient(l, k) * BinomialCoefficient(lk, l);
    return a * b * c + LegendreIntegralFactor(m, l, x, k + 1);
}

auto LegendrePolynomial(int m, int l, float x)
{
    auto a = pow(-1, m) * pow(2, l) * pow(1 - x * x, m / 2.f);
    auto b = LegendreIntegralFactor(m, l, x, m);
    return a * b;
}

int main(int argc, char** argv)
{
    float x = 2;
    for (auto m= 0u; m < 2; ++m) {
        for (auto l = 0u; l < 2; ++l) {
            std::cout << "m : " << m << " l : " << l << ' ' << LegendrePolynomial(m, l, x) << std::endl;
        }
    }
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
            //fpsCamera->SetZfar(1000);
            auto assets(AssetsParser::Parse(filePath.string()));
            auto assetCameras = assets->GetComponents<Camera>();
            s_cameras.push_back(fpsCamera);
            s_cameras.insert(s_cameras.end(), assetCameras.begin(), assetCameras.end());
            auto scene = std::static_pointer_cast<Scene>(assets->GetComponent<Scene>());
            if (scene == nullptr) {
                return -43;
            }
            scene->SetCurrentCamera(fpsCamera);
            auto dirLight =  Component::Create<DirectionnalLight>("MainLight", glm::vec3(0.5), glm::vec3(1, 10, 1), false);
            dirLight->SetHalfSize(glm::vec3(50));
            //auto pointLight = Component::Create<PointLight>("PointLight", glm::vec3(1, 0, 0));
            //pointLight->SetPosition(glm::vec3(0, 0.1, 0));
           // pointLight->SetPower(2);
            //pointLight->SetParent(fpsCamera);
            //s_light = dirLight;
            scene->Add(dirLight);
            //scene->Add(pointLight);
            //pointLight->SetParent(scene->CurrentCamera());
            auto newEnv = Component::Create<Environment>("Environment");
            newEnv->SetDiffuse(Component::Create<Cubemap>("EnvironmentCube", TextureParser::parse("Environment", (Engine::ResourcePath() / "env/diffuse.hdr").string())));
            newEnv->SetIrradiance(Component::Create<Cubemap>("EnvironmentDiffuse", TextureParser::parse("EnvironmentDiffuse", (Engine::ResourcePath() / "env/environment.hdr").string())));
            scene->SetEnvironment(newEnv);
            Scene::SetCurrent(scene);
        //}
        SetupCallbacks();
        Engine::Start();
    }
    _CrtDumpMemoryLeaks();
    return 0;
}