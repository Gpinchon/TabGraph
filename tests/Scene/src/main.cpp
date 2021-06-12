/*
* @Author: gpinchon
* @Date:   2020-08-09 19:54:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-26 18:33:52
*/
#define USE_HIGH_PERFORMANCE_GPU
#include <DLLExport.hpp>

#include <Animation/Animation.hpp>
#include <Assets/Asset.hpp>
#include <Assets/AssetsParser.hpp>
#include <Assets/Image.hpp>
#include <Camera/FPSCamera.hpp>
#include <Config.hpp>
#include <Engine.hpp>
#include <Event/EventsManager.hpp>
#include <Event/GameController.hpp>
#include <Event/InputDevice/GameController.hpp>
#include <Event/InputDevice/Keyboard.hpp>
#include <Event/InputDevice/Mouse.hpp>
#include <Light/DirectionalLight.hpp>
#include <Light/HDRLight.hpp>
#include <Light/PointLight.hpp>
#include <Light/SkyLight.hpp>
#include <Material/Material.hpp>
#include <Scene/Scene.hpp>
#include <StackTracer.hpp>
#include <Surface/Skybox.hpp>
#include <Texture/TextureCubemap.hpp>
#include <Tools/Tools.hpp>
#include <Window.hpp>

#include <csignal>
#include <filesystem>

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
            auto fpsCamera = Component::Create<FPSCamera>("main_camera");
            //fpsCamera->SetZfar(1000);
            auto asset { Component::Create<Asset>(filePath) };
            AssetsParser::AddParsingTask({
                AssetsParser::ParsingTask::Type::Sync,
                asset
            });
            auto assetCameras = asset->GetComponents<Camera>();
            s_cameras.push_back(fpsCamera);
            s_cameras.insert(s_cameras.end(), assetCameras.begin(), assetCameras.end());
            auto scene = std::static_pointer_cast<Scene>(asset->GetComponent<Scene>());
            if (scene == nullptr) {
                return -43;
            }
            scene->SetCurrentCamera(fpsCamera);
            auto newEnv = Component::Create<Skybox>("Skybox");
            auto diffuseAsset { Component::Create<Asset>(Engine::GetResourcePath() / "env/diffuse.jpg") };
            diffuseAsset->parsingOptions.image.maximumResolution = 2048;
            newEnv->SetTexture(Component::Create<TextureCubemap>(diffuseAsset));
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
                if (!event.state)
                    return;
                if (event.key == SPEEDDOWNK)
                    --speed;
                else if (event.key == SPEEDUPK)
                    ++speed;
                speed = std::max(1.f, speed);
            };
            auto quitCB = [engine = std::weak_ptr(engine)](const Event::Keyboard&) { engine.lock()->Stop(); };
            auto changeCameraCB = [engine = std::weak_ptr(engine)](const Event::Keyboard& event) {
                if (engine.lock()->GetCurrentScene() == nullptr)
                    return;
                if (!event.state || event.repeat)
                    return;
                static auto s_currentCamera = 0u;
                s_currentCamera++;
                if (s_currentCamera >= s_cameras.size())
                    s_currentCamera = 0;
                engine.lock()->GetCurrentScene()->SetCurrentCamera(s_cameras.at(s_currentCamera));
                s_light->SetParent(engine.lock()->GetCurrentScene()->CurrentCamera());
            };
            auto animationCB = [engine = std::weak_ptr(engine)](const Event::Keyboard& event) {
                if (engine.lock()->GetCurrentScene() == nullptr)
                    return;
                if (!event.state || event.repeat || engine.lock()->GetCurrentScene()->GetComponents<Animation>().empty())
                    return;
                static auto animations = engine.lock()->GetCurrentScene()->GetComponents<Animation>();
                static auto currentAnimation(animations.begin());
                (*currentAnimation)->Stop();
                currentAnimation++;
                if (currentAnimation == animations.end())
                    currentAnimation = animations.begin();
                (*currentAnimation)->SetRepeat(true);
                (*currentAnimation)->Play();
            };
            auto wheelCB = [engine = std::weak_ptr(engine)](const Event::MouseWheel& event) {
                if (engine.lock()->GetCurrentScene() == nullptr)
                    return;
                auto scene { engine.lock()->GetCurrentScene() };
                auto proj = scene->CurrentCamera()->GetProjection();
                if (proj.type == Camera::Projection::Type::PerspectiveInfinite) {
                    auto perspectiveInfinite = proj.Get<Camera::Projection::PerspectiveInfinite>();
                    perspectiveInfinite.fov -= event.amount.y;
                    perspectiveInfinite.fov = glm::clamp(perspectiveInfinite.fov, 1.f, 70.f);
                    scene->CurrentCamera()->SetProjection(perspectiveInfinite);
                }
                else if (proj.type == Camera::Projection::Type::Perspective) {
                    auto perspective = proj.Get<Camera::Projection::Perspective>();
                    perspective.fov -= event.amount.y;
                    perspective.fov = glm::clamp(perspective.fov, 1.f, 70.f);
                    scene->CurrentCamera()->SetProjection(perspective);
                }
            };
            auto moveCB = [engine = std::weak_ptr(engine)](const Event::MouseMove& event) {
                if (engine.lock()->GetCurrentScene() == nullptr)
                    return;
                auto camera = std::dynamic_pointer_cast<FPSCamera>(engine.lock()->GetCurrentScene()->CurrentCamera());
                if (camera == nullptr)
                    return;
                static glm::vec3 cameraRotation {};
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
            auto refreshCB = [engine = std::weak_ptr(engine)](float delta) {
                if (engine.lock()->GetCurrentScene() == nullptr)
                    return;
                auto camera = std::dynamic_pointer_cast<FPSCamera>(engine.lock()->GetCurrentScene()->CurrentCamera());
                if (camera == nullptr)
                    return;
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
            GameController::OnButtonDown(0, GameController::Button::A).Connect([](const Event::GameControllerButton& event) {
                std::cout << (event.state ? "Button pressed " : "Button released ") << int(event.button) << " on Controller " << event.id << std::endl;
                GameController::Rumble(0, 0.5, 0.5, 500);
            });
            Keyboard::OnKey(SPEEDUPK).Connect(speedCB);
            Keyboard::OnKey(SPEEDDOWNK).Connect(speedCB);
            Keyboard::OnKey(QUITK).Connect(quitCB);
            Keyboard::OnKey(Keyboard::Key::Return).Connect(FullscreenCallback);
            Keyboard::OnKey(Keyboard::Key::Q).Connect(CallbackQuality);
            Keyboard::OnKey(Keyboard::Key::A).Connect(animationCB);
            Keyboard::OnKey(Keyboard::Key::C).Connect(changeCameraCB);
            Mouse::OnMove().Connect(moveCB);
            Mouse::OnWheel().Connect(wheelCB);
            engine->OnFixedUpdate().Connect(refreshCB);
            window->OnEvent(Event::Window::Type::SizeChanged).Connect(
                [engine = std::weak_ptr(engine)](const Event::Window& event) {
                    if (engine.lock()->GetCurrentScene() == nullptr)
                        return;
                    auto scene{ engine.lock()->GetCurrentScene() };
                    auto proj = scene->CurrentCamera()->GetProjection();
                    if (proj.type == Camera::Projection::Type::PerspectiveInfinite) {
                        auto perspectiveInfinite = proj.Get<Camera::Projection::PerspectiveInfinite>();
                        perspectiveInfinite.aspectRatio = event.window->GetSize().x / float(event.window->GetSize().y);
                        scene->CurrentCamera()->SetProjection(perspectiveInfinite);
                    }
                    else if (proj.type == Camera::Projection::Type::Perspective) {
                        auto perspective = proj.Get<Camera::Projection::Perspective>();
                        perspective.aspectRatio = event.window->GetSize().x / float(event.window->GetSize().y);
                        scene->CurrentCamera()->SetProjection(perspective);
                }
            });
        }
        engine->Start();
    }
    _CrtDumpMemoryLeaks();
    return 0;
}