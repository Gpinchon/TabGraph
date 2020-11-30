#pragma once

#include "Light/Light.hpp"

class PointLight : public Light {
public:
	PointLight(const std::string& name, glm::vec3 color, glm::vec3 position, float power, bool cast_shadow = false);
	virtual void render_shadow() = 0;
	virtual void Draw() = 0;
};