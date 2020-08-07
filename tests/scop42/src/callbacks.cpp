/*
* @Author: gpi
* @Date:   2019-03-26 13:04:37
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-06-21 10:54:57
*/

#include "Common.hpp"
#include "Input/Events.hpp" // for Events
#include "Node.hpp" // for Node
#include "glm/glm.hpp" // for s_vec3, s_vec2, glm::vec3, glm::clamp
#include "Callback.hpp"
#include "Config.hpp"
#include "Engine.hpp" // for Stop
#include "Environment.hpp" // for Environment
#include "Camera/FPSCamera.hpp" // for FPSCamera
#include "Input/GameController.hpp" // for Controller, GameController
#include "Input/Keyboard.hpp" // for Keyboard
#include "Mesh/Mesh.hpp" // for Mesh
#include "Input/Mouse.hpp" // for Mouse
#include "Render.hpp" // for InternalQuality, SetInternalQua...
#include "Scene/Scene.hpp"
#include "Transform.hpp"
#include <SDL2/SDL_events.h> // for SDL_KeyboardEvent, SDL_Controll...
#include <SDL2/SDL_gamecontroller.h> // for SDL_CONTROLLER_AXIS_LEFTX, SDL_...
#include <SDL2/SDL_scancode.h> // for SDL_SCANCODE_KP_MINUS, SDL_SCAN...
#include <Tools.hpp>
#include <Window.hpp> // for Window
#include <csignal> // for raise, SIGSEGV
#include <iostream> // for operator<<, endl, basic_ostream
#include <math.h> // for M_PI
#include <memory> // for shared_ptr, dynamic_pointer_cast
#include "scop.hpp" // for DOWNK, LEFTK, MouseMoveCallback

static auto cameraRotation = glm::vec3(M_PI / 2.f, M_PI / 2.f, 5.f);
bool orbit = false;

void callback_camera(SDL_Event *)
{
    auto scene(Scene::Current());
    auto camera = std::dynamic_pointer_cast<FPSCamera>(scene->GetCameraByName("main_camera"));
    auto controller = GameController::Get(0);
    glm::vec2 raxis = glm::vec2(0, 0);
    glm::vec2 laxis = glm::vec2(0, 0);
    float taxis = 0;
    Mouse::set_relative(SDL_TRUE);
    if (controller->is_connected())
    {
        raxis.x = controller->axis(SDL_CONTROLLER_AXIS_RIGHTX);
        raxis.y = -controller->axis(SDL_CONTROLLER_AXIS_RIGHTY);
        laxis.x = -controller->axis(SDL_CONTROLLER_AXIS_LEFTX);
        laxis.y = controller->axis(SDL_CONTROLLER_AXIS_LEFTY);
        taxis += controller->axis(SDL_CONTROLLER_AXIS_TRIGGERLEFT);
        taxis -= controller->axis(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
        cameraRotation.x += raxis.x * Events::delta_time() * Config::Get("GamePadSensitivity", 100.f);
        cameraRotation.y += raxis.y * Events::delta_time() * Config::Get("GamePadSensitivity", 100.f);
        camera->SetYaw(cameraRotation.x);
        camera->SetPitch(cameraRotation.y);
    }
    else
    {
        laxis.x = Keyboard::key(LEFTK) - Keyboard::key(RIGHTK);
        laxis.y = Keyboard::key(DOWNK) - Keyboard::key(UPK);
        raxis.x = 0;
        raxis.y = Keyboard::key(ZOOMK) - Keyboard::key(UNZOOMK);
        taxis += Keyboard::key(SDL_SCANCODE_PAGEUP);
        taxis -= Keyboard::key(SDL_SCANCODE_PAGEDOWN);
    }
    cameraRotation.z -= laxis.y * Events::delta_time();
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->Position() - float(Events::delta_time() * laxis.x) * camera->GetComponent<Transform>()->Right());
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->Position() - float(Events::delta_time() * laxis.y) * camera->GetComponent<Transform>()->Forward());
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->Position() + float(Events::delta_time() * taxis) * Common::Up());
}

void callback_scale(SDL_KeyboardEvent *event)
{
    static float scale = 1;

    if (event == nullptr || event->type == SDL_KEYUP)
    {
        return;
    }
    if (Keyboard::key(SDL_SCANCODE_LCTRL) == 0u)
    {
        return;
    }
    if (Keyboard::key(SDL_SCANCODE_KP_PLUS) != 0u)
    {
        scale += (0.005 * (Keyboard::key(SDL_SCANCODE_LSHIFT) + 1));
    }
    else if (Keyboard::key(SDL_SCANCODE_KP_MINUS) != 0u)
    {
        scale -= (0.005 * (Keyboard::key(SDL_SCANCODE_LSHIFT) + 1));
    }
    scale = glm::clamp(scale, 0.0001f, 1000.f);
    mainMesh->GetComponent<Transform>()->SetScale(glm::vec3(scale));
}

void switch_background()
{
    static int b = 0;
    b++;
    auto env = Environment::Get(b);
    if (env != nullptr)
    {
        Environment::set_current(env);
    }
    else
    {
        b = 0;
        env = Environment::Get(b);
        Environment::set_current(env);
    }
}

void callback_background(SDL_KeyboardEvent *event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
    {
        return;
    }
    switch_background();
}

void controller_callback_background(SDL_ControllerButtonEvent *event)
{
    if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN))
    {
        return;
    }
    switch_background();
}

void controller_callback_quality(SDL_ControllerButtonEvent *event)
{
    if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN))
    {
        return;
    }
    GameController::Get(0)->rumble(0.5, 100);
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

void callback_quality(SDL_KeyboardEvent *event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
    {
        return;
    }
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

bool rotate_model = true;

void controller_callback_rotation(SDL_ControllerButtonEvent *event)
{
    if (event == nullptr || (event->type != SDL_CONTROLLERBUTTONDOWN && event->type != SDL_JOYBUTTONDOWN))
    {
        return;
    }
    rotate_model = !rotate_model;
}

void keyboard_callback_rotation(SDL_KeyboardEvent *event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
    {
        return;
    }
    rotate_model = !rotate_model;
}

void callback_refresh(SDL_Event * /*unused*/)
{
    callback_camera(nullptr);
    if (mainMesh == nullptr)
    {
        return;
    }
    if (rotate_model)
    {
        static float rotation = 0;
        rotation += Events::delta_time();
        rotation = CYCLE(rotation, 0, 360);
        mainMesh->GetComponent<Transform>()->SetRotation(glm::vec3(0, rotation, 0));
    }
}

void callback_exit(SDL_KeyboardEvent * /*unused*/)
{
    Engine::Stop();
}

void callback_fullscreen(SDL_KeyboardEvent *event)
{
    if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u))
    {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::fullscreen(fullscreen);
    }
    (void)event;
}

void MouseWheelCallback(SDL_MouseWheelEvent *event)
{
    auto scene(Scene::Current());
    static auto camera = std::dynamic_pointer_cast<FPSCamera>(scene->GetCameraByName("main_camera"));
    camera->SetFov(camera->Fov() - event->y * 0.01);
    camera->SetFov(glm::clamp(camera->Fov(), 1.0f, 70.f));
}

void MouseMoveCallback(SDL_MouseMotionEvent *event)
{
    if (GameController::Get(0)->is_connected())
        return;
    auto scene(Scene::Current());
    static auto camera = std::dynamic_pointer_cast<FPSCamera>(scene->GetCameraByName("main_camera"));
    cameraRotation.x -= event->xrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
    cameraRotation.y -= event->yrel * Events::delta_time() * Config::Get("MouseSensitivity", 2.f);
    camera->SetYaw(cameraRotation.x);
    camera->SetPitch(cameraRotation.y);
}

void callback_crash(SDL_KeyboardEvent *)
{
    std::cout << "CRASH !!!" << std::endl;
    std::raise(SIGSEGV);
}

void setup_callbacks()
{
    Keyboard::set_callback(SDL_SCANCODE_KP_PLUS, callback_scale);
    Keyboard::set_callback(SDL_SCANCODE_KP_MINUS, callback_scale);
    Keyboard::set_callback(SDL_SCANCODE_SPACE, callback_background);
    Keyboard::set_callback(SDL_SCANCODE_ESCAPE, callback_exit);
    Keyboard::set_callback(SDL_SCANCODE_RETURN, callback_fullscreen);
    Keyboard::set_callback(SDL_SCANCODE_Q, callback_quality);
    Keyboard::set_callback(SDL_SCANCODE_R, keyboard_callback_rotation);
    //Keyboard::set_callback(SDL_SCANCODE_C, callback_crash);
    Mouse::set_relative(SDL_TRUE);
    Mouse::set_move_callback(MouseMoveCallback);
    Mouse::set_wheel_callback(MouseWheelCallback);
    Events::AddRefreshCallback(Callback<void()>::Create(callback_refresh, nullptr));
    auto controller = GameController::Get(0);
    if (controller == nullptr)
        return;
    controller->set_button_callback(SDL_CONTROLLER_BUTTON_A, controller_callback_quality);
    controller->set_button_callback(SDL_CONTROLLER_BUTTON_B, controller_callback_rotation);
    controller->set_button_callback(SDL_CONTROLLER_BUTTON_Y, controller_callback_background);
}
