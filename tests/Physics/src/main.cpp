#define USE_HIGH_PERFORMANCE_GPU
#include "DLLExport.hpp"

#include "Animation/Animation.hpp"
#include "Callback.hpp"
#include "Camera/FPSCamera.hpp"
#include "Config.hpp"
#include "Engine.hpp"
#include "Input/Events.hpp"
#include "Input/Keyboard.hpp"
#include "Input/Mouse.hpp"
#include "Light/Light.hpp"
#include "Mesh/CubeMesh.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/PlaneMesh.hpp"
#include "Mesh/SphereMesh.hpp"
#include "Physics/RigidBody.hpp"
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
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->WorldPosition() - float(Events::delta_time() * laxis.x * 10) * camera->GetComponent<Transform>()->Right());
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->WorldPosition() - float(Events::delta_time() * laxis.y * 10) * camera->GetComponent<Transform>()->Forward());
    camera->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->WorldPosition() + float(Events::delta_time() * taxis * 10) * Common::Up());
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
    auto camera = std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera());
    if (camera == nullptr)
        return;
    camera->SetFov(camera->Fov() - event->y);
    camera->SetFov(glm::clamp(camera->Fov(), 1.0f, 70.f));
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
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
        return;
    static auto currentAnimation(0);
    Scene::Current()->Animations().at(currentAnimation)->Stop();
    currentAnimation++;
    currentAnimation = currentAnimation % Scene::Current()->Animations().size();
    Scene::Current()->Animations().at(currentAnimation)->SetRepeat(true);
    Scene::Current()->Animations().at(currentAnimation)->Play();
}

void ExitCallback(SDL_KeyboardEvent*)
{
    Engine::Stop();
}

#include "Material.hpp"
#include "Physics/BoundingAABB.hpp"
#include "Physics/BoundingMesh.hpp"
#include "Physics/BoundingPlane.hpp"
#include "Physics/BoundingSphere.hpp"

void ThrowBall(SDL_KeyboardEvent* event)
{
    if (event == nullptr || (event->type == SDL_KEYUP || (event->repeat != 0u)))
        return;
    auto camera = Scene::Current()->CurrentCamera();
    auto dir = camera->GetComponent<Transform>()->Forward();
    static auto geometry = *SphereMesh::Create("BallMesh", 1.f)->Geometrys().begin();
    static auto material = Material::Create("BallMaterial");
    auto mesh(Mesh::Create());
    auto node(Node::Create("node"));
    auto rigidBody(RigidBody::Create("cubeRigidBody", node, BoundingSphere::Create(glm::vec3(0.f), 1.f)));
    mesh->AddGeometry(geometry);
    mesh->AddMaterial(material);
    node->GetComponent<Transform>()->SetPosition(camera->GetComponent<Transform>()->WorldPosition());
    node->AddComponent(mesh);
    rigidBody->SetMass(1);
    rigidBody->SetApplyGravity(true);
    rigidBody->ApplyLocalPush(dir * 100.f, glm::vec3(0.f));
    Scene::Current()->Add(rigidBody);
    Scene::Current()->Add(node);
}

void SetupCallbacks()
{
    Keyboard::set_callback(SDL_SCANCODE_ESCAPE, ExitCallback);
    Keyboard::set_callback(SDL_SCANCODE_RETURN, FullscreenCallback);
    Keyboard::set_callback(SDL_SCANCODE_Q, CallbackQuality);
    Keyboard::set_callback(SDL_SCANCODE_A, CallbackAnimation);
    Keyboard::set_callback(SDL_SCANCODE_SPACE, ThrowBall);
    //Mouse::set_relative(SDL_TRUE);
    Mouse::set_move_callback(MouseMoveCallback);
    Mouse::set_wheel_callback(MouseWheelCallback);
    Events::AddRefreshCallback(Callback<void()>::Create(CameraCallback, std::dynamic_pointer_cast<FPSCamera>(Scene::Current()->CurrentCamera())));
}

static inline void CreateCubes(unsigned nbr, std::shared_ptr<Scene> scene)
{
    auto geometry = *CubeMesh::Create("originalMesh", glm::vec3(1.f))->Geometrys().begin();
    auto rowLength = sqrt(nbr);
    for (auto i = 0u; i < rowLength; ++i) {
        for (auto j = 0u; j < rowLength; ++j) {
            glm::vec3 position((i - (rowLength / 2.f)) * 1.1f, (j + 1) * 1.1f, 0);
            std::cout << position.x << ' ' << position.y << ' ' << position.z << '\n';
            auto mesh(Mesh::Create());
            auto material(Material::Create("mesh_" + std::to_string(i) + "_material"));
            auto node(Node::Create("node"));
            auto rigidBody(RigidBody::Create("cubeRigidBody", node, BoundingAABB::Create(glm::vec3(-0.5), glm::vec3(0.5))));
            //auto rigidBody(RigidBody::Create("cubeRigidBody", node, BoundingMesh::Create(mesh)));
            material->SetAlbedo(glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f));
            //rigidBody->SetStatic(true);
            rigidBody->SetApplyGravity(false);
            rigidBody->SetMass(1.f);
            mesh->AddGeometry(geometry);
            mesh->AddMaterial(material);
            node->GetComponent<Transform>()->SetPosition(position);
            node->AddComponent(mesh);
            scene->Add(rigidBody);
            scene->Add(node);
        }
    }

    /*auto geometry = *SphereMesh::Create("originalMesh", 1.f)->Geometrys().begin();
    for (auto i = 0u; i < nbr; ++i) {
        auto mesh(Mesh::Create());
        auto material(Material::Create("mesh_" + std::to_string(i) + "_material"));
        auto node(Node::Create("node"));
        auto rigidBody(RigidBody::Create("cubeRigidBody", node, BoundingSphere::Create(glm::vec3(0.f), 1.f)));
        //auto rigidBody(RigidBody::Create("cubeRigidBody", node, BoundingAABB::Create(glm::vec3(-0.5), glm::vec3(0.5))));
        //auto rigidBody(RigidBody::Create("cubeRigidBody", node, BoundingMesh::Create(mesh)));
        material->SetAlbedo(glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f));
        material->SetRoughness(0.1f);
        mesh->AddGeometry(geometry);
        mesh->AddMaterial(material);
        node->GetComponent<Transform>()->SetPosition(glm::vec3(rand() % 50 - 25, rand() % 20 + 1, rand() % 50 - 25));
        //node->GetComponent<Transform>()->SetPosition(glm::vec3(0, 2, -10));
        node->AddComponent(mesh);
        //mesh->SetImpostor(rigidBody);

        rigidBody->SetMass(1);
        //mesh->SetMass((rand() % 100 - 50) / 100.f);
        rigidBody->SetApplyGravity(true);
        rigidBody->ApplyWorldPush(glm::vec3(0, 10, -1), glm::vec3(0, 0, 1), node->GetComponent<Transform>()->WorldPosition());
        rigidBody->SetLinearVelocity(glm::vec3(0));
        //rigidBody->ApplyCentralPush(glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50));
        //mesh->SetApplyGravity(rand() % 2);
        scene->Add(rigidBody);
        scene->Add(node);
    }*/
    auto cubeMesh(CubeMesh::Create("cubeMesh", glm::vec3(1, 1, 1)));
    auto cubeNode(Node::Create("cubeNode"));

    auto rigidBody(RigidBody::Create("cubeRigidBody", cubeNode, BoundingSphere::Create(glm::vec3(0), 1.f)));
    cubeMesh->GetMaterial(0)->SetAlbedo(glm::vec3(rand() % 255 / 255.f, rand() % 255 / 255.f, rand() % 255 / 255.f));
    cubeNode->GetComponent<Transform>()->SetPosition(glm::vec3(0, 5, 0));
    cubeNode->AddComponent(cubeMesh);
    //cubeMesh->SetImpostor(rigidBody);

    rigidBody->SetMass(1);
    //cubeMesh->SetMass((rand() % 100 - 50) / 100.f);
    rigidBody->SetApplyGravity(false);
    rigidBody->ApplyWorldPush(glm::vec3(0, 10, -1), glm::vec3(0, 0, 1), cubeNode->GetComponent<Transform>()->WorldPosition());
    rigidBody->SetLinearVelocity(glm::vec3(0));
    //rigidBody->ApplyCentralPush(1.f / normalize(cubeMesh->Position()) * 2.f);
    //rigidBody->ApplyCentralPush(glm::vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50));
    //cubeMesh->SetApplyGravity(rand() % 2);
    scene->Add(rigidBody);
    scene->Add(cubeNode);
}

#include "Physics/IntersectFunctions.hpp"

void CollidersTest()
{
    //auto sphere1(RigidBody::Create("sphere1", Node::Create("sphere1_Node"), BoundingSphere::Create(glm::vec3(0, 0, 0), 1)));
    //auto sphere2(RigidBody::Create("sphere2", Node::Create("sphere2_Node"), BoundingSphere::Create(glm::vec3(0, 3, 0), 1)));
    //auto sphere3(RigidBody::Create("sphere3", Node::Create("sphere3_Node"), BoundingSphere::Create(glm::vec3(0, 0, 2), 1)));
    //auto sphere4(RigidBody::Create("sphere4", Node::Create("sphere4_Node"), BoundingSphere::Create(glm::vec3(1, 0, 0), 1)));
    //
    //auto aIntersectB(sphere1->Collides(sphere2));
    //auto aIntersectC(sphere1->Collides(sphere3));
    //auto aIntersectD(sphere1->Collides(sphere4));
    //std::cout << "sphere1 intersect sphere2 : " << aIntersectB.GetIntersects()
    //          << ", distance : " << aIntersectB.GetDistance() << std::endl;
    //std::cout << "sphere1 intersect sphere3 : " << aIntersectC.GetIntersects()
    //          << ", distance : " << aIntersectC.GetDistance() << std::endl;
    //std::cout << "sphere1 intersect sphere4 : " << aIntersectD.GetIntersects()
    //          << ", distance : " << aIntersectD.GetDistance() << std::endl;
    //
    //auto AABB1(RigidBody::Create("AABB1", Node::Create("AABB1_Node"), BoundingAABB::Create(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1))));
    //auto AABB2(RigidBody::Create("AABB2", Node::Create("AABB2_Node"), BoundingAABB::Create(glm::vec3(1, 1, 1), glm::vec3(2, 2, 2))));
    //auto AABB3(RigidBody::Create("AABB3", Node::Create("AABB3_Node"), BoundingAABB::Create(glm::vec3(1, 0, 0), glm::vec3(2, 1, 1))));
    //auto AABB4(RigidBody::Create("AABB4", Node::Create("AABB4_Node"), BoundingAABB::Create(glm::vec3(0, 0, -2), glm::vec3(1, 1, -1))));
    //auto AABB5(RigidBody::Create("AABB5", Node::Create("AABB5_Node"), BoundingAABB::Create(glm::vec3(0, 0.5, 0), glm::vec3(1, 1.5, 1))));
    //
    //auto AABB1IntersectAABB2(AABB1->Collides(AABB2));
    //auto AABB1IntersectAABB3(AABB1->Collides(AABB3));
    //auto AABB1IntersectAABB4(AABB1->Collides(AABB4));
    //auto AABB1IntersectAABB5(AABB1->Collides(AABB5));
    //std::cout << "AABB1 intersect AABB2 : " << AABB1IntersectAABB2.GetIntersects()
    //          << ", distance : " << AABB1IntersectAABB2.GetDistance() << std::endl;
    //std::cout << "AABB1 intersect AABB3 : " << AABB1IntersectAABB3.GetIntersects()
    //          << ", distance : " << AABB1IntersectAABB3.GetDistance() << std::endl;
    //std::cout << "AABB1 intersect AABB4 : " << AABB1IntersectAABB4.GetIntersects()
    //          << ", distance : " << AABB1IntersectAABB4.GetDistance() << std::endl;
    //std::cout << "AABB1 intersect AABB5 : " << AABB1IntersectAABB5.GetIntersects()
    //          << ", distance : " << AABB1IntersectAABB5.GetDistance() << std::endl;
    /*
    BoundingPlane plane1(glm::vec3(0, 1, 0), 0);

    auto plane1IntersectSphere1(plane1.Intersect(sphere1));
    auto plane1IntersectSphere2(plane1.Intersect(sphere2));
    auto plane1IntersectSphere3(plane1.Intersect(sphere3));
    auto plane1IntersectSphere4(plane1.Intersect(sphere4));
    std::cout << "plane1 intersect sphere1 : " << plane1IntersectSphere1.GetIntersects()
              << ", distance : " << plane1IntersectSphere1.GetDistance() << std::endl;
    std::cout << "plane1 intersect sphere2 : " << plane1IntersectSphere2.GetIntersects()
              << ", distance : " << plane1IntersectSphere2.GetDistance() << std::endl;
    std::cout << "plane1 intersect sphere3 : " << plane1IntersectSphere3.GetIntersects()
              << ", distance : " << plane1IntersectSphere3.GetDistance() << std::endl;
    std::cout << "plane1 intersect sphere4 : " << plane1IntersectSphere4.GetIntersects()
              << ", distance : " << plane1IntersectSphere4.GetDistance() << std::endl;
              */
}

void CreateColliders(std::shared_ptr<Scene>& scene)
{
    auto planeMesh(PlaneMesh::Create("PlaneMesh", glm::vec2(100, 100)));
    auto planeNode(Node::Create("PlaneNode"));
    auto rigidBody(RigidBody::Create("PlaneRigidBody", planeNode, BoundingAABB::Create(glm::vec3(-50, 0, -50), glm::vec3(50, 0, 50))));
    planeNode->AddComponent(planeMesh);
    scene->Add(planeNode);
    scene->Add(rigidBody);
    CreateCubes(50, scene);
}

int main(int, char**)
{
    CollidersTest();
    //return 0;
    Config::Parse(Engine::ResourcePath() + "config.ini");
    Engine::Init();
    auto scene(Scene::Create("mainScene"));
    scene->Add(DirectionnalLight::Create("MainLight", glm::vec3(1, 1, 1), glm::vec3(10, 10, 10), 0.5, false));
    scene->SetCurrentCamera(FPSCamera::Create("main_camera", 45));
    scene->CurrentCamera()->GetComponent<Transform>()->SetPosition(glm::vec3 { 0, 5, 10 });
    CreateColliders(scene);
    Scene::SetCurrent(scene);
    SetupCallbacks();
    Engine::Start();
    return 0;
}
