#pragma once

#include "Light/Light.hpp"

class PointLight : public Light {
	PROPERTY(float, Radius, 1);
	PROPERTY(float, Cutoff, 0.1);
public:
	PointLight(const std::string& name, glm::vec3 color, bool cast_shadow = false);
	virtual void render_shadow();
	virtual void Draw();
};