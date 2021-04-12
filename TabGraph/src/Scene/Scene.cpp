/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:58:53
*/

#include "Scene/Scene.hpp"
#include "Animation/Animation.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Engine.hpp"
#include "Light/Light.hpp"
#include "Node.hpp"
#include "Physics/BoundingAABB.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Physics/RigidBody.hpp"
#include "Renderer/SceneRenderer.hpp"
#include "Skybox.hpp"

#include <algorithm>
#include <iostream>

Renderer::SceneRenderer& Scene::GetRenderer()
{
    return *_renderer;
}

Scene::Scene(const std::string& name)
    : Component(name)
    , _renderer(new Renderer::SceneRenderer(*this))
{
}

Scene::Scene(const Scene& other)
    : Component(other)
    , _up(other._up)
    , _aabb(std::static_pointer_cast<BoundingAABB>(other._aabb ? other._aabb->Clone() : nullptr))
    , _currentCamera(other._currentCamera)
    , _skyboxIndex(other._skyboxIndex)
    , _renderer(new Renderer::SceneRenderer(*this))
{
}

void Scene::AddRootNode(std::shared_ptr<Node> node)
{
    AddComponent(node);
}

void Scene::Remove(std::shared_ptr<Node> node)
{
    if (node == nullptr)
        return;
    RemoveComponent(node);
}

void Scene::Add(std::shared_ptr<RigidBody> rigidBody)
{
    auto physicsEngine = GetComponent<PhysicsEngine>();
    if (physicsEngine != nullptr)
        physicsEngine->AddRigidBody(rigidBody);
}

void Scene::Add(std::shared_ptr<Node> node)
{
    if (node == nullptr)
        return;
    AddComponent(node);
    if (auto camera = std::dynamic_pointer_cast<Camera>(node); camera != nullptr)
        AddComponent(camera);
    else if (auto light = std::dynamic_pointer_cast<Light>(node); light != nullptr)
        AddComponent(light);
}

void Scene::Add(std::shared_ptr<Animation> animation)
{
    AddComponent(animation);
}

std::shared_ptr<Scene>& CurrentScene()
{
    static std::shared_ptr<Scene> current(nullptr);
    return current;
}

std::shared_ptr<Scene> Scene::Current()
{
    return CurrentScene();
}

void Scene::SetCurrent(std::shared_ptr<Scene> scene)
{
    CurrentScene() = scene;
    Common::SetUp(scene->Up());
}

std::shared_ptr<Camera> Scene::CurrentCamera() const
{
    return GetComponent<Camera>(_currentCamera);
}

void Scene::SetCurrentCamera(std::shared_ptr<Camera> camera)
{
    /*auto cameras = Cameras();
    if (std::find(cameras.begin(), cameras.end(), camera) == cameras.end())
        Add(camera);*/
    _currentCamera = AddComponent(camera);
}

template <typename T>
std::shared_ptr<T> GetByName(const std::string& name, const std::shared_ptr<T> rootNode)
{
    if (rootNode == nullptr || rootNode->Name() == name)
        return std::dynamic_pointer_cast<T>(rootNode);
    //for (const auto object : Nodes()) {
    //	auto result(std::dynamic_pointer_cast<T>(GetByName<Node>(name, object)));
    //	if (result != nullptr)
    //		return result;
    //}
    return nullptr;
}

template <typename T>
std::shared_ptr<T> GetByPointer(std::shared_ptr<Node> node, const std::shared_ptr<T> rootNode)
{
    if (rootNode == nullptr || rootNode == node)
        return std::dynamic_pointer_cast<T>(rootNode);
    //for (const auto object : Nodes()) {
    //	auto result(std::dynamic_pointer_cast<T>(GetByPointer<Node>(node, object)));
    //	if (result != nullptr)
    //		return result;
    //}
    return nullptr;
}

std::shared_ptr<Skybox> Scene::GetSkybox() const
{
    return GetComponent<Skybox>(_skyboxIndex);
}

void Scene::SetSkybox(const std::shared_ptr<Skybox>& env)
{
    _skyboxIndex = AddComponent(env);
    SkyboxChanged(env);
}

glm::vec3 Scene::Up() const
{
    return _up;
}

void Scene::SetUp(glm::vec3 up)
{
    _up = up;
}

std::shared_ptr<BoundingAABB> Scene::GetLimits() const
{
    return _aabb;
}

void Renderer::SceneRendererDeleter::operator()(Renderer::SceneRenderer* renderer)
{
    delete renderer;
}
