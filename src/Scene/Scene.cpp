
#include "Animation/Animation.hpp"
#include "Engine.hpp"
#include "Camera/Camera.hpp"
#include "Common.hpp"
#include "Light/Light.hpp"
#include "Node.hpp"
#include "Scene/Scene.hpp"
#include "Physics/RigidBody.hpp"
#include "Transform.hpp"
#include <iostream>
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <algorithm>

std::shared_ptr<Scene> Scene::Create(const std::string &name)
{
	return std::shared_ptr<Scene>(new Scene(name));
}

Scene::Scene(const std::string &name) : _name(name) {

}

std::string Scene::Name() const
{
	return _name;
}

void Scene::SetName(const std::string &name)
{
	_name = name;
}

void Scene::_AddNodeChildren(std::shared_ptr<Node> node)
{
	for (auto index(0u); index < node->ChildCount(); ++index) {
		auto child(node->GetChild(index));
		if (std::find(_nodes.begin(), _nodes.end(), child) == _nodes.end())
			_nodes.push_back(child);
		else
			return;
		if (auto camera = std::dynamic_pointer_cast<Camera>(child); camera != nullptr)
			_cameras.push_back(camera);
		else if (auto light = std::dynamic_pointer_cast<Light>(child); light != nullptr)
			_lights.push_back(light);
		_AddNodeChildren(child);
	}
}

void Scene::AddRootNode(std::shared_ptr<Node> node)
{
	_nodes.push_back(node);
	_rootNodes.push_back(node);
	_AddNodeChildren(node);
}

void Scene::Add(std::shared_ptr<RigidBody> rigidBody)
{
	_physicsEngine.AddRigidBody(rigidBody);
}

void Scene::Add(std::shared_ptr<Node> node)
{
	if (node == nullptr)
		return;
	if (std::find(_nodes.begin(), _nodes.end(), node) == _nodes.end()) {
		_nodes.push_back(node);
		_rootNodes.push_back(node);
	}
	else
		return;
	if (auto camera = std::dynamic_pointer_cast<Camera>(node); camera != nullptr)
		_cameras.push_back(camera);
	else if (auto light = std::dynamic_pointer_cast<Light>(node); light != nullptr)
		_lights.push_back(light);
}

void Scene::Add(std::shared_ptr<Animation> animation)
{
	if (animation == nullptr)
		return;
	_animations.push_back(animation);
}

//void UpdateTransformMatrix(std::shared_ptr<Node> rootNode)
//{
//	if (rootNode == nullptr)
//        return;
//    rootNode->UpdateTransformMatrix();
//    for (const auto &child : Nodes())
//        UpdateTransformMatrix(child);
//}
//
void NodesFixedUpdate(std::shared_ptr<Node> rootNode)
{
	if (rootNode == nullptr)
        return;
    rootNode->FixedUpdate();
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        NodesFixedUpdate(rootNode->GetChild(index));
}

void Scene::FixedUpdate()
{
	Common::SetUp(Up());
	for (auto &animation: Animations()) {
		if (animation->Playing())
			animation->Advance();
	}
	PhysicsUpdate();
	//for (auto &node : RootNodes())
	//	UpdateTransformMatrix(node);
    for (auto &node : RootNodes())
    	NodesFixedUpdate(node);
}

void Scene::PhysicsUpdate()
{
	/*for (auto &node : RootNodes())
		NodesPhysicsUpdate(node);*/
	static auto time(SDL_GetTicks());
	time = SDL_GetTicks();
	static auto lastTime(time);
	_physicsEngine.Simulate((time - lastTime) / 1000.f);
	lastTime = time;
	_physicsEngine.CheckCollision();
	//for (auto &node : RootNodes())
	//	NodesCollisionCheck(node);
}

void NodesUpdateGPU(std::shared_ptr<Node> rootNode)
{
	if (rootNode == nullptr)
        return;
    rootNode->UpdateGPU();
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        NodesUpdateGPU(rootNode->GetChild(index));
    //for (const auto &child : Nodes())
    //    NodesUpdateGPU(child);
}

void Scene::UpdateGPU()
{
	for (auto &node : RootNodes())
		NodesUpdateGPU(node);
}

void NodesUpdate(std::shared_ptr<Node> rootNode)
{
	if (rootNode == nullptr)
        return;
    rootNode->Update();
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        NodesUpdate(rootNode->GetChild(index));
}

void Scene::Update()
{
	for (auto &node : RootNodes())
		NodesUpdate(node);
}

void DrawNodes(std::shared_ptr<Node> rootNode, RenderMod mode) {
    if (rootNode == nullptr)
        return;
    rootNode->Draw(mode);
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        DrawNodes(rootNode->GetChild(index), mode);
}

void Scene::Render(const RenderMod &mode) {
    for (auto node : Scene::Current()->RootNodes())
        DrawNodes(node, mode);
}

void DrawNodesDepth(std::shared_ptr<Node> rootNode, RenderMod mode) {
	if (rootNode == nullptr)
        return;
    rootNode->DrawDepth(mode);
    for (auto index = 0u; index < rootNode->ChildCount(); ++index)
        DrawNodesDepth(rootNode->GetChild(index), mode);
}

void Scene::RenderDepth(const RenderMod &mode) {
    for (auto node : Scene::Current()->RootNodes())
        DrawNodesDepth(node, mode);
}

std::shared_ptr<Scene> &CurrentScene()
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
}

std::shared_ptr<Camera> Scene::CurrentCamera() const
{
	return _currentCamera;
}

void Scene::SetCurrentCamera(std::shared_ptr<Camera> camera)
{
	if (std::find(_cameras.begin(), _cameras.end(), camera) == _cameras.end())
		Add(camera);
	_currentCamera = camera;
}

template<typename T>
std::shared_ptr<T> GetByName(const std::string &name, const std::shared_ptr<T> rootNode) {
	if (rootNode == nullptr || rootNode->Name() == name)
		return std::dynamic_pointer_cast<T>(rootNode);
	//for (const auto object : Nodes()) {
	//	auto result(std::dynamic_pointer_cast<T>(GetByName<Node>(name, object)));
	//	if (result != nullptr)
	//		return result;
	//}
	return nullptr;
}

template<typename T>
std::shared_ptr<T> GetByPointer(std::shared_ptr<Node> node, const std::shared_ptr<T> rootNode) {
	if (rootNode == nullptr || rootNode == node)
		return std::dynamic_pointer_cast<T>(rootNode);
	//for (const auto object : Nodes()) {
	//	auto result(std::dynamic_pointer_cast<T>(GetByPointer<Node>(node, object)));
	//	if (result != nullptr)
	//		return result;
	//}
	return nullptr;
}

std::shared_ptr<Node> Scene::GetNode(std::shared_ptr<Node> node) const
{
	for (const auto &object : _nodes) {
		auto result(GetByPointer(node, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

std::shared_ptr<Node> Scene::GetNodeByName(const std::string &name) const
{
	for (const auto &object : _nodes) {
		auto result(GetByName(name, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

std::shared_ptr<Light> Scene::GetLightByName(const std::string &name) const
{
	for (const auto &object : _lights) {
		auto result(GetByName(name, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

std::shared_ptr<Camera> Scene::GetCameraByName(const std::string &name) const
{
	for (const auto &object : _cameras) {
		auto result(GetByName(name, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

const std::vector<std::shared_ptr<Node>> &Scene::RootNodes()
{
	return _rootNodes;
}

const std::vector<std::shared_ptr<Node>> &Scene::Nodes()
{
	return _nodes;
}

const std::vector<std::shared_ptr<Light>> &Scene::Lights()
{
	return _lights;
}

const std::vector<std::shared_ptr<Camera>> &Scene::Cameras()
{
	return _cameras;
}

const std::vector<std::shared_ptr<Animation>> &Scene::Animations()
{
	return _animations;
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