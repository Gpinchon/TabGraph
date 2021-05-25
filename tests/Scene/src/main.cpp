/*
* @Author: gpinchon
* @Date:   2020-08-09 19:54:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:24
*/
#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include <Animation/Animation.hpp>
#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>
#include <Camera/FPSCamera.hpp>
#include <Config.hpp>
#include <Engine.hpp>
#include <Event/EventsManager.hpp>
#include <Event/InputDevice/GameController.hpp>
#include <Event/InputDevice/Keyboard.hpp>
#include <Event/InputDevice/Mouse.hpp>
#include <Light/DirectionalLight.hpp>
#include <Material/Material.hpp>
#include <Surface/CubeMesh.hpp>
#include <Surface/Mesh.hpp>
#include <Scene/Scene.hpp>
#include <Tools/Tools.hpp>
#include <Window.hpp>

#define QUITK Keyboard::Key::Escape
#define DOWNK Keyboard::Key::PageDown
#define UPK Keyboard::Key::PageUp
#define FORWARDK Keyboard::Key::Up
#define BACKWARDK Keyboard::Key::Down
#define LEFTK Keyboard::Key::Left
#define RIGHTK Keyboard::Key::Right
#define SPEEDUPK Keyboard::Key::NumpadPlus
#define SPEEDDOWNK Keyboard::Key::NumpadMinus

float speed = 1.f;

void FullscreenCallback(const Event::Keyboard& event)
{
    if (event.key == Keyboard::Key::Return && event.alt && !event.repeat && event.state) {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        event.window->SetFullscreen(fullscreen);
    }
}

void CallbackQuality(const Event::Keyboard& event)
{
    if (!event.state || event.repeat)
        return;
    auto quality = CYCLE(Config::Global().Get("InternalQuality", 1.f) + 0.25f, 0.5f, 1.5f);
    Config::Global().Set("InternalQuality", quality);
}

std::shared_ptr<Light> s_light;
std::vector<std::shared_ptr<Camera>> s_cameras;

#include "Event/GameController.hpp"

#include "Assets/Image.hpp"
#include "Light/HDRLight.hpp"
#include "Light/PointLight.hpp"
#include "Light/SkyLight.hpp"
#include "Surface/Skybox.hpp"
#include "StackTracer.hpp"
#include "Texture/TextureCubemap.hpp"
#include <csignal>
#include <filesystem>

int main(int argc, char** argv)
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    if (argc <= 1)
        return -42;
    {
        StackTracer::set_signal_handler(SIGABRT);
        StackTracer::set_signal_handler(SIGSEGV);
        Config::Global().Parse(Engine::GetResourcePath() / "config.ini");
        auto window = Window::Create(Config::Global().Get("WindowName", std::string("")), Config::Global().Get("WindowSize", glm::vec2(1280, 720)));
        auto renderer = Renderer::FrameRenderer::Create(window);
        auto engine = Engine::Create(renderer);
        renderer->SetSwapInterval(Renderer::SwapInterval(Config::Global().Get("SwapInterval", 0)));        
        std::filesystem::path filePath = (std::string(argv[1]));
        if (!filePath.is_absolute()) {
            filePath = Engine::GetExecutionPath() / filePath;
        }
        {
            std::cout << filePath << std::endl;
            auto fpsCamera = Component::Create<FPSCamera>("main_camera", 45);
            //fpsCamera->SetZfar(1000);
            auto asset { Component::Create<Asset>("file:" + filePath.string()) };
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
            auto diffuseAsset { Component::Create<Asset>("file:" + (Engine::GetResourcePath() / "env/diffuse.jpg").string()) };
            newEnv->SetTexture(Component::Create<TextureCubemap>(diffuseAsset)); //"EnvironmentCube", TextureParser::parse("Skybox", (Engine::ResourcePath() / "env/diffuse.hdr").string())));
            scene->SetSkybox(newEnv);

            auto hdrLight { Component::Create<HDRLight>(diffuseAsset) };
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
            engine->SetCurrentScene(scene);
        }
        //setup callbacks
        {
            auto speedCB = [](const Event::Keyboard& event) {
                if (!event.state) return;
                if (event.key == SPEEDDOWNK) --speed;
                else if (event.key == SPEEDUPK) ++speed;
                speed = std::max(1.f, speed);
            };
            auto quitCB = [engine](const Event::Keyboard&) { engine->Stop(); };
            auto changeCameraCB = [engine](const Event::Keyboard& event) {
                if (!event.state || event.repeat) return;
                static auto s_currentCamera = 0u;
                s_currentCamera++;
                if (s_currentCamera >= s_cameras.size())
                    s_currentCamera = 0;
                engine->GetCurrentScene()->SetCurrentCamera(s_cameras.at(s_currentCamera));
                s_light->SetParent(engine->GetCurrentScene()->CurrentCamera());
            };
            auto animationCB = [engine](const Event::Keyboard& event) {
                if (!event.state || event.repeat || engine->GetCurrentScene()->GetComponents<Animation>().empty())
                    return;
                static auto animations = engine->GetCurrentScene()->GetComponents<Animation>();
                static auto currentAnimation(animations.begin());
                (*currentAnimation)->Stop();
                currentAnimation++;
                if (currentAnimation == animations.end()) currentAnimation = animations.begin();
                (*currentAnimation)->SetRepeat(true);
                (*currentAnimation)->Play();
            };
            auto wheelCB = [engine](const Event::MouseWheel& event) {
                auto scene{ engine->GetCurrentScene() };
                scene->CurrentCamera()->SetFov(scene->CurrentCamera()->Fov() - event.amount.y);
                scene->CurrentCamera()->SetFov(glm::clamp(scene->CurrentCamera()->Fov(), 1.0f, 70.f));
            };
            auto moveCB = [engine](const Event::MouseMove& event) {
                auto camera = std::dynamic_pointer_cast<FPSCamera>(engine->GetCurrentScene()->CurrentCamera());
                if (camera == nullptr) return;
                static glm::vec3 cameraRotation{};
                if (Mouse::GetButtonState(Mouse::Button::Left)) {
                    cameraRotation.x -= event.relative.x * 0.05 * Config::Global().Get("MouseSensitivity", 2.f);
                    cameraRotation.y -= event.relative.y * 0.05 * Config::Global().Get("MouseSensitivity", 2.f);
                }
                if (Mouse::GetButtonState(Mouse::Button::Right))
                    cameraRotation.z += event.relative.x * 0.05 * Config::Global().Get("MouseSensitivity", 2.f);
                camera->SetYaw(cameraRotation.x);
                camera->SetPitch(cameraRotation.y);
                camera->SetRoll(cameraRotation.z);
            };
            auto refreshCB = [engine](float delta) {
                auto camera = std::dynamic_pointer_cast<FPSCamera>(engine->GetCurrentScene()->CurrentCamera());
                if (camera == nullptr) return;
                glm::vec2 laxis = glm::vec2(0, 0);
                float taxis = 0;
                //Mouse::set_relative(SDL_TRUE);
                laxis.x = Keyboard::GetKeyState(LEFTK) - Keyboard::GetKeyState(RIGHTK);
                laxis.y = Keyboard::GetKeyState(BACKWARDK) - Keyboard::GetKeyState(FORWARDK);
                taxis += Keyboard::GetKeyState(UPK);
                taxis -= Keyboard::GetKeyState(DOWNK);
                camera->SetPosition(camera->GetPosition() - float(delta * laxis.x * speed) * camera->Right());
                camera->SetPosition(camera->GetPosition() - float(delta * laxis.y * speed) * camera->Forward());
                camera->SetPosition(camera->GetPosition() + float(delta * taxis * speed) * Common::Up());
            };
            GameController::OnButton(0, GameController::Button::A).Connect([](const Event::GameControllerButton& event) {
                std::cout << (event.state ? "Button pressed " : "Button released ") << int(event.button) << " on Controller " << event.id << std::endl;
            });
            Keyboard::OnKey(SPEEDUPK).Connect(speedCB);
            Keyboard::OnKey(SPEEDDOWNK).Connect(speedCB);
            Keyboard::OnKey(QUITK).Connect(quitCB);
            Keyboard::OnKey(Keyboard::Key::Return).Connect(FullscreenCallback);
            Keyboard::OnKey(Keyboard::Key::Q).Connect(CallbackQuality);
            Keyboard::OnKey(Keyboard::Key::A).Connect(animationCB);
            Keyboard::OnKey(Keyboard::Key::C).Connect(changeCameraCB);
            //Mouse::set_relative(SDL_TRUE);
            Mouse::OnMove().Connect(moveCB);
            Mouse::OnWheel().Connect(wheelCB);
            engine->OnFixedUpdate().Connect(refreshCB);
        }
        engine->Start();
    }
    _CrtDumpMemoryLeaks();
    return 0;
}