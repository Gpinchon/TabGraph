#pragma once

#include "vml.h"
#include <limits>

struct	BoundingElement
{
    BoundingElement() : 
        min(new_vec3(std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max())),
        max(new_vec3(std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min())),
        center(new_vec3(0, 0, 0)) {};
	virtual bool collides(const BoundingElement &) = 0;
	VEC3	min;
	VEC3	max;
	VEC3	center;
};
