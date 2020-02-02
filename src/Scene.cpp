#include "Scene.hpp"

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

std::vector<std::shared_ptr<Node>> &Scene::Nodes()
{
	return _nodes;
}

std::vector<std::shared_ptr<Camera>> &Scene::Cameras()
{
	return _cameras;
}
