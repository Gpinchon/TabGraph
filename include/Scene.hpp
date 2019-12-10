#pragma once

#include "Node.hpp"
#include <string>

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
private:
	std::string _name;
	std::vector<std::shared_ptr<Node>> _nodes;
	
};