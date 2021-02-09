/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-19 13:07:58
*/

#include "Scene/Scene.hpp"
#include "Animation/Animation.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Engine.hpp"
#include "Light/Light.hpp"
#include "Node.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Physics/RigidBody.hpp"
#include "Environment.hpp"

#include <algorithm>
#include <iostream>

Scene::Scene(const std::string& name)
    : _name(name)
{
}

std::string Scene::Name() const
{
    return _name;
}

void Scene::SetName(const std::string& name)
{
    _name = name;
}

void Scene::AddRootNode(std::shared_ptr<Node> node)
{
    AddComponent(node);
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

void Scene::Render(const RenderPass&pass, const RenderMod& mode)
{
    for (auto node : Scene::Current()->GetComponents<Node>())
        node->Draw(std::static_pointer_cast<Scene>(shared_from_this()), pass, mode, true);
}

void Scene::RenderDepth(const RenderMod& mode)
{
    for (auto node : Scene::Current()->GetComponents<Node>())
        node->DrawDepth(std::static_pointer_cast<Scene>(shared_from_this()), mode, true);
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
/*
std::shared_ptr<Node> Scene::GetNode(std::shared_ptr<Node> node) const
{
    for (const auto& object : Nodes()) {
        auto result(GetByPointer(node, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

std::shared_ptr<Node> Scene::GetNodeByName(const std::string& name) const
{
    for (const auto& object : Nodes()) {
        auto result(GetByName(name, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

std::shared_ptr<Light> Scene::GetLightByName(const std::string& name) const
{
    for (const auto& object : Lights()) {
        auto result(GetByName(name, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}

std::shared_ptr<Camera> Scene::GetCameraByName(const std::string& name) const
{
    for (const auto& object : Cameras()) {
        auto result(GetByName(name, object));
        if (result != nullptr)
            return result;
    }
    return nullptr;
}
*/
std::shared_ptr<Environment> Scene::GetEnvironment() const
{
    return GetComponent<Environment>(_environmentIndex);
}

void Scene::SetEnvironment(const std::shared_ptr<Environment>& env)
{
    _environmentIndex = AddComponent(env);
    EnvironmentChanged(env);
}

glm::vec3 Scene::Up() const
{
    return _up;
}

void Scene::SetUp(glm::vec3 up)
{
    _up = up;
}

std::shared_ptr<AABB> Scene::GetLimits() const
{
    return _aabb;
}