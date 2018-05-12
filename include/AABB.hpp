#pragma once

#include "BoundingElement.hpp"

struct	AABB : public BoundingElement
{
    AABB() : BoundingElement() {};
	bool	collides(const BoundingElement &) { return (false); };
};
