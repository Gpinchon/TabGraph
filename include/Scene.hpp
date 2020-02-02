#pragma once

#include "Node.hpp"
#include <string>

class Camera;

/**
 * @brief scene container
 */
struct Scene
{
	Scene(const std::string &name);
	~Scene() = default;
	std::string Name() const;
	void SetName(const std::string &name);
	std::vector<std::shared_ptr<Node>> &Nodes();
	std::vector<std::shared_ptr<Camera>> &Cameras();
private:
	std::string _name;
	std::vector<std::shared_ptr<Node>> _nodes;
	std::vector<std::shared_ptr<Camera>> _cameras;
	
};