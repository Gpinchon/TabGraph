#pragma once

#include "Node.hpp"
#include "vml.h"

struct	Camera : public Node
{
	static Camera	&create(const std::string &, float fov);
	void			orbite(float phi, float theta, float radius);
	void			update();
	MAT4		view;
	MAT4		projection;
	FRUSTUM		frustum;
	float		fov;
	Node		*target;
private :
	Camera(const std::string &name);
};