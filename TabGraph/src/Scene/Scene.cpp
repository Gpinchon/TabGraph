/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-28 07:58:53
*/

#include <Scene/Scene.hpp>
#include <Animation/Animation.hpp>
#include <Camera/Camera.hpp>
#include <Common.hpp>
#include <Engine.hpp>
#include <Light/Light.hpp>
#include <Node.hpp>
#include <Physics/BoundingAABB.hpp>
#include <Physics/PhysicsEngine.hpp>
#include <Physics/RigidBody.hpp>
#include <Surface/Skybox.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Renderer/SceneRenderer.hpp>
#endif

#include <Component.hpp>

#include <algorithm>
#include <iostream>

Renderer::SceneRenderer& Scene::GetRenderer()
{
    return *_renderer;
}

Scene::Scene(const std::string& name)
    : Object(name)
    , _renderer(new Renderer::SceneRenderer(*this))
{
}

Scene::Scene(const Scene& other)
    : Object(other)
    , _rootNode(Component::Create<Node>())
    , _up(other._up)
    , _aabb(std::static_pointer_cast<BoundingAABB>(other._aabb ? other._aabb->Clone() : nullptr))
    , _camera(other._camera)
    , _skybox(other._skybox)
    , _renderer(new Renderer::SceneRenderer(*this))
{
}

void Scene::Remove(std::shared_ptr<Node> node)
{
    if (node == nullptr)
        return;
    _rootNode->RemoveChild(node);
}

void Scene::Add(std::shared_ptr<RigidBody> rigidBody)
{
    if (_physicsEngine != nullptr)
        _physicsEngine->AddRigidBody(rigidBody);
}

void Scene::Add(std::shared_ptr<Animation> animation)
{
    _animations.insert(animation);
}

std::shared_ptr<Scene>& CurrentScene()
{
    static std::shared_ptr<Scene> current(nullptr);
    return current;
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

void Scene::SetCamera(std::shared_ptr<Camera> camera)
{
    _rootNode->RemoveChild(GetCamera());
    _camera = camera;
    _rootNode->AddChild(camera);
}

void Scene::SetSkybox(std::shared_ptr<Skybox> skybox)
{
    _rootNode->RemoveSurface(GetSkybox());
    _skybox = skybox;
    _rootNode->AddSurface(skybox);
}

std::shared_ptr<BoundingAABB> Scene::GetLimits() const
{
    return _aabb;
}

void Renderer::SceneRendererDeleter::operator()(Renderer::SceneRenderer* renderer)
{
    delete renderer;
}
