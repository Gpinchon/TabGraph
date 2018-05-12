#pragma once

#include "Node.hpp"

struct	Light : public Node
{
	int8_t		type;
	int8_t		cast_shadow;
	Framebuffer		*render_buffer;
};

struct PointLight : public Light
{
	VEC3		color;
	float		power;
	float		attenuation;
	float		falloff;
};

struct DirectionnalLight : public Light
{
	VEC3		color;
	float		power;
};
