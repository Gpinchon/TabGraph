#include "Engine.hpp"
#include "Animation.hpp"
#include "Config.hpp"
#include "FPSCamera.hpp"
#include "Light.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "Events.hpp"
#include "Callback.hpp"
#include "Scene.hpp"
#include "SceneParser.hpp"
#include "Window.hpp"
#include "Mesh.hpp"
#include "CubeMesh.hpp"
#include "PlaneMesh.hpp"
#include "Tools.hpp"
#include "Render.hpp"
#include "RigidBody.hpp"
#include "parser/GLTF.hpp"

#define DOWNK SDL_SCANCODE_DOWN
#define UPK SDL_SCANCODE_UP
#define LEFTK SDL_SCANCODE_LEFT
#define RIGHTK SDL_SCANCODE_RIGHT
#define ZOOMK SDL_SCANCODE_KP_PLUS
#define UNZOOMK SDL_SCANCODE_KP_MINUS

void CameraCallback(std::shared_ptr<FPSCamera> camera)
{
    //auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
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
    camera->SetPosition(camera->Position() - float(Events::delta_time() * laxis.x * 10) * camera->Right());
    camera->SetPosition(camera->Position() - float(Events::delta_time() * laxis.y * 10) * camera->Forward());
    camera->SetPosition(camera->Position() + float(Events::delta_time() * taxis * 10) * Common::Up());
}

void MouseMoveCallback(SDL_MouseMotionEvent *event)
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

void MouseWheelCallback(SDL_MouseWheelEvent *event)
{
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    camera->SetFov(camera->Fov() - event->y);
    camera->SetFov(glm::clamp(camera->Fov(), 1.0f, 70.f));
}

void FullscreenCallback(SDL_KeyboardEvent*)
{
    if ((Keyboard::key(SDL_SCANCODE_RETURN) != 0u) && (Keyboard::key(SDL_SCANCODE_LALT) != 0u))
    {
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        Window::fullscreen(fullscreen);
    }
}

void CallbackQuality(SDL_KeyboardEvent *event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
        return;
    Render::SetInternalQuality(CYCLE(Render::InternalQuality() + 0.25, 0.5, 1.5));
}

void CallbackAnimation(SDL_KeyboardEvent *event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
        return;
    static auto currentAnimation(0);
    Scene::Current()->Animations().at(currentAnimation)->Stop();
    currentAnimation++;
    currentAnimation = currentAnimation % Scene::Current()->Animations().size();
    Scene::Current()->Animations().at(currentAnimation)->SetRepeat(true);
    Scene::Current()->Animations().at(currentAnimation)->Play();
}

void ExitCallback(SDL_KeyboardEvent* ) {
    Engine::Stop();
}

void SetupCallbacks()
{
    Keyboard::set_callback(SDL_SCANCODE_ESCAPE, ExitCallback);
    Keyboard::set_callback(SDL_SCANCODE_RETURN, FullscreenCallback);
    Keyboard::set_callback(SDL_SCANCODE_Q, CallbackQuality);
    Keyboard::set_callback(SDL_SCANCODE_A, CallbackAnimation);
    //Mouse::set_relative(SDL_TRUE);
    Mouse::set_move_callback(MouseMoveCallback);
    Mouse::set_wheel_callback(MouseWheelCallback);
    Events::AddRefreshCallback(Callback<void()>::Create(CameraCallback, std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera())));
}

#include "Material.hpp"
#include "BoundingAABB.hpp"

static inline void CreateCubes(unsigned /*nbr*/, std::shared_ptr<Scene> scene)
{
    /*for (auto i = 0u; i < nbr; ++i)
    {
        auto cube(CubeMesh::Create("cubeMesh", glm::vec3(1, 1, 1)));
        auto rigidBody(RigidBody::Create("cubeRigidBody"));
        cube->GetMaterial(0)->SetAlbedo(glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f));
        cube->SetPosition(glm::vec3(rand() % 100 - 50, rand() % 100, rand() % 100 - 50));

        cube->SetImpostor(rigidBody);
        rigidBody->SetMass(1);
        //cube->SetMass((rand() % 100 - 50) / 100.f);
        //rigidBody->SetApplyGravity(true);
        //rigidBody->ApplyLocalPush(normalize(cube->Position()) * 5.f, glm::vec3(0, 0, 1) - cube->Position());
        rigidBody->ApplyWorldPush(1.f / normalize(cube->Position()), glm::vec3(0, 0, 0), cube->Position());
        rigidBody->SetLinearVelocity(glm::vec3(0));
        //rigidBody->ApplyCentralPush(1.f / normalize(cube->Position()) * 2.f);
        //rigidBody->ApplyCentralPush(glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50));
        //cube->SetApplyGravity(rand() % 2);
        
        scene->Add(cube);
    }*/
    auto cube(CubeMesh::Create("cubeMesh", glm::vec3(1, 1, 1)));
    auto rigidBody(RigidBody::Create("cubeRigidBody", cube, BoundingSphere::Create(glm::vec3(0), 1.f)));
    cube->GetMaterial(0)->SetAlbedo(glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f));
    cube->SetPosition(glm::vec3(0, 5, 0));
    //cube->SetImpostor(rigidBody);

    rigidBody->SetMass(1);
    //cube->SetMass((rand() % 100 - 50) / 100.f);
    rigidBody->SetApplyGravity(false);
    rigidBody->ApplyWorldPush(glm::vec3(0, 10, -1), glm::vec3(0, 0, 1), cube->Position());
    rigidBody->SetLinearVelocity(glm::vec3(0));
    //rigidBody->ApplyCentralPush(1.f / normalize(cube->Position()) * 2.f);
    //rigidBody->ApplyCentralPush(glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50));
    //cube->SetApplyGravity(rand() % 2);
    scene->Add(rigidBody);
    scene->Add(cube);
}

#include "BoundingSphere.hpp"
#include "BoundingAABB.hpp"
#include "BoundingPlane.hpp"


void CollidersTest()
{
    auto sphere1(BoundingSphere::Create(glm::vec3(0, 0, 0), 1));
    auto sphere2(BoundingSphere::Create(glm::vec3(0, 3, 0), 1));
    auto sphere3(BoundingSphere::Create(glm::vec3(0, 0, 2), 1));
    auto sphere4(BoundingSphere::Create(glm::vec3(1, 0, 0), 1));

    auto aIntersectB(sphere1->Intersect(sphere2));
    auto aIntersectC(sphere1->Intersect(sphere3));
    auto aIntersectD(sphere1->Intersect(sphere4));
    std::cout   << "Sphare sphere1 intersect sphere2 : " << aIntersectB.GetIntersects()
                << ", distance : " << aIntersectB.GetDistance() << std::endl;
    std::cout   << "Sphare sphere1 intersect sphere3 : " << aIntersectC.GetIntersects()
                << ", distance : " << aIntersectC.GetDistance() << std::endl;
    std::cout   << "Sphare sphere1 intersect sphere4 : " << aIntersectD.GetIntersects()
                << ", distance : " << aIntersectD.GetDistance() << std::endl;

    auto AABB1(BoundingAABB::Create(glm::vec3(0, 0,   0), glm::vec3(1, 1,   1)));
    auto AABB2(BoundingAABB::Create(glm::vec3(1, 1,   1), glm::vec3(2, 2,   2)));
    auto AABB3(BoundingAABB::Create(glm::vec3(1, 0,   0), glm::vec3(2, 1,   1)));
    auto AABB4(BoundingAABB::Create(glm::vec3(0, 0,  -2), glm::vec3(1, 1,  -1)));
    auto AABB5(BoundingAABB::Create(glm::vec3(0, 0.5, 0), glm::vec3(1, 1.5, 1)));

    auto AABB1IntersectAABB2(AABB1->Intersect(AABB2));
    auto AABB1IntersectAABB3(AABB1->Intersect(AABB3));
    auto AABB1IntersectAABB4(AABB1->Intersect(AABB4));
    auto AABB1IntersectAABB5(AABB1->Intersect(AABB5));
    std::cout   << "AABB1 intersect AABB2 : " << AABB1IntersectAABB2.GetIntersects()
                << ", distance : " << AABB1IntersectAABB2.GetDistance() << std::endl;
    std::cout   << "AABB1 intersect AABB3 : " << AABB1IntersectAABB3.GetIntersects()
                << ", distance : " << AABB1IntersectAABB3.GetDistance() << std::endl;
    std::cout   << "AABB1 intersect AABB4 : " << AABB1IntersectAABB4.GetIntersects()
                << ", distance : " << AABB1IntersectAABB4.GetDistance() << std::endl;
    std::cout   << "AABB1 intersect AABB5 : " << AABB1IntersectAABB5.GetIntersects()
                << ", distance : " << AABB1IntersectAABB5.GetDistance() << std::endl;
    
    BoundingPlane plane1(glm::vec3(0, 1, 0), 0);

    auto plane1IntersectSphere1(plane1.Intersect(sphere1));
    auto plane1IntersectSphere2(plane1.Intersect(sphere2));
    auto plane1IntersectSphere3(plane1.Intersect(sphere3));
    auto plane1IntersectSphere4(plane1.Intersect(sphere4));
    std::cout   << "plane1 intersect sphere1 : " << plane1IntersectSphere1.GetIntersects()
                << ", distance : " << plane1IntersectSphere1.GetDistance() << std::endl;
    std::cout   << "plane1 intersect sphere2 : " << plane1IntersectSphere2.GetIntersects()
                << ", distance : " << plane1IntersectSphere2.GetDistance() << std::endl;
    std::cout   << "plane1 intersect sphere3 : " << plane1IntersectSphere3.GetIntersects()
                << ", distance : " << plane1IntersectSphere3.GetDistance() << std::endl;
    std::cout   << "plane1 intersect sphere4 : " << plane1IntersectSphere4.GetIntersects()
                << ", distance : " << plane1IntersectSphere4.GetDistance() << std::endl;
}


int main(int, char **)
{
    //CollidersTest();
    Config::Parse(Engine::ResourcePath() + "config.ini");
    Engine::Init();
	auto scene(Scene::Create("mainScene"));
    scene->Add(DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), 0.5, false));
    scene->SetCurrentCamera(FPSCamera::Create("main_camera", 45));
    scene->CurrentCamera()->SetPosition(glm::vec3{0, 5, 10});
    CreateCubes(1, scene);
    scene->Add(PlaneMesh::Create("PlaneMesh", glm::vec2(100, 100)));
    Scene::SetCurrent(scene);
    SetupCallbacks();
    Engine::Start();
	return 0;
}