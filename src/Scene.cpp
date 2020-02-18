#include "Scene.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Node.hpp"
#include "Common.hpp"

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

void Scene::Add(std::shared_ptr<Node> node)
{
	if (node == nullptr)
		return;
	_nodes.insert(node);
	if (auto camera = std::dynamic_pointer_cast<Camera>(node); camera != nullptr)
		_cameras.insert(camera);
	else if (auto light = std::dynamic_pointer_cast<Light>(node); light != nullptr)
		_lights.insert(light);
}

void UpdateTransformMatrix(std::shared_ptr<Node> rootNode)
{
	if (rootNode == nullptr)
        return;
    rootNode->UpdateTransformMatrix();
    for (const auto &child : rootNode->Children())
        UpdateTransformMatrix(child);
}

void NodesFixedUpdate(std::shared_ptr<Node> rootNode)
{
	if (rootNode == nullptr)
        return;
    rootNode->FixedUpdate();
    for (const auto &child : rootNode->Children())
        NodesFixedUpdate(child);
}

void Scene::FixedUpdate()
{
	Common::SetUp(Up());
	for (auto &node : Nodes())
		UpdateTransformMatrix(node);
    for (auto &node : Nodes())
    	NodesFixedUpdate(node);
}

void NodesUpdate(std::shared_ptr<Node> rootNode)
{
	if (rootNode == nullptr)
        return;
    rootNode->Update();
    for (const auto &child : rootNode->Children())
        NodesUpdate(child);
}

void Scene::Update()
{
	for (auto &node : Nodes())
		NodesUpdate(node);
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

std::shared_ptr<Camera> Scene::CurrentCamera()
{
	return _currentCamera;
}

void Scene::SetCurrentCamera(std::shared_ptr<Camera> camera)
{
	Add(camera);
	_currentCamera = camera;
}

template<typename T>
std::shared_ptr<T> GetByName(const std::string &name, const std::shared_ptr<T> rootNode) {
	if (rootNode == nullptr || rootNode->Name() == name)
		return std::dynamic_pointer_cast<T>(rootNode);
	for (const auto object : rootNode->Children()) {
		auto result(std::dynamic_pointer_cast<T>(GetByName<Node>(name, object)));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

std::shared_ptr<Node> Scene::GetNodeByName(const std::string &name)
{
	for (const auto object : _nodes) {
		auto result(GetByName(name, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

std::shared_ptr<Light> Scene::GetLightByName(const std::string &name)
{
	for (const auto object : _lights) {
		auto result(GetByName(name, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

std::shared_ptr<Camera> Scene::GetCameraByName(const std::string &name)
{
	for (const auto object : _cameras) {
		auto result(GetByName(name, object));
		if (result != nullptr)
			return result;
	}
	return nullptr;
}

const std::set<std::shared_ptr<Node>> &Scene::Nodes()
{
	return _nodes;
}

const std::set<std::shared_ptr<Light>> &Scene::Lights()
{
	return _lights;
}

const std::set<std::shared_ptr<Camera>> &Scene::Cameras()
{
	return _cameras;
}

glm::vec3 Scene::Up() const
{
	return _up;
}

void Scene::SetUp(glm::vec3 up)
{
	_up = up;
}

void DrawNodes(std::shared_ptr<Node> rootNode, RenderMod mode) {
    if (rootNode == nullptr)
        return;
    rootNode->Draw(mode);
    for (const auto &child : rootNode->Children())
        DrawNodes(child, mode);
}

void Scene::Render(const RenderMod &mode) {
    for (auto node : Scene::Current()->Nodes())
        DrawNodes(node, mode);
}

void DrawNodesDepth(std::shared_ptr<Node> rootNode, RenderMod mode) {
    rootNode->DrawDepth(mode);
    std::shared_ptr<Node> child;
    for (const auto &child : rootNode->Children())
        DrawNodesDepth(child, mode);
}

void Scene::RenderDepth(const RenderMod &mode) {
    for (auto node : Scene::Current()->Nodes())
        DrawNodesDepth(node, mode);
}
