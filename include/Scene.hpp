#pragma once

#include "Object.hpp"

/**
 * @brief scene container
 */
class Scene : public Object
{
public:
	Scene(const std::string &name);
	~Scene() = default;
	
};