/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 21:42:11 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/15 19:37:24 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Light.hpp"
#include "Engine.hpp"
#include "Framebuffer.hpp"

Light::Light(const std::string &name) : Node(name) {
}

Light		*Light::create(const std::string &name, VEC3 color, VEC3 position, float power)
{
	auto	light = new Light(name);
	light->color() = color;
	light->position() = position;
	light->power() = power;
	Engine::add(*light);
	return (light);
}

Framebuffer	*Light::render_buffer()
{
	return (_render_buffer);
}

VEC3		&Light::color()
{
	return (_color);
}

float		&Light::power()
{
	return (_power);
}

bool		&Light::cast_shadow()
{
	return (_cast_shadow);
}

LightType	Light::type()
{
	return (Point);
}

void		Light::render_shadow()
{
	
}

DirectionnalLight::DirectionnalLight(const std::string &name) : Light(name) {
}

DirectionnalLight	*DirectionnalLight::create(const std::string &name, VEC3 color, VEC3 position, float power, bool cast_shadow)
{
	auto	light = new DirectionnalLight(name);
	light->color() = color;
	light->position() = position;
	light->power() = power;
	light->cast_shadow() = cast_shadow;
	if (cast_shadow) {
		light->_render_buffer = Framebuffer::create(light->name() + "_shadowMap", new_vec2(CFG::ShadowRes(), CFG::ShadowRes()), nullptr, 0, 1);
		light->_render_buffer->depth()->set_parameteri(GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		light->_render_buffer->depth()->set_parameteri(GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	Engine::add(*light);
	return (light);
}

void		DirectionnalLight::render_shadow()
{
	
}

void		DirectionnalLight::fixed_update()
{
	auto	proj = mat4_orthographic(new_frustum(-10, 10, -10, 10), 0.1, 100);
	mat4_transform() = mat4_mult_mat4(proj, mat4_lookat(position(), new_vec3(0, 0 , 0), UP));
}

LightType	DirectionnalLight::type()
{
	return (Directionnal);
}
