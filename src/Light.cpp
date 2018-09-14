/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 21:42:11 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/14 17:27:22 by gpinchon         ###   ########.fr       */
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
	if (cast_shadow)
		light->_render_buffer = Framebuffer::create(light->name() + "_shadowMap", new_vec2(CFG::ShadowRes(), CFG::ShadowRes()), nullptr, 0, 1);
	Engine::add(*light);
	return (light);
}

LightType	DirectionnalLight::type()
{
	return (Directionnal);
}

void		DirectionnalLight::render_shadow()
{
	
}