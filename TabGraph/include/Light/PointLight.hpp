#pragma once

#include "Light/Light.hpp"

class PointLight : public Light {
	PROPERTY(float, Radius, 1);
	PROPERTY(float, Power, 1);
public:
	PointLight(const std::string& name, glm::vec3 color, bool cast_shadow = false);
};