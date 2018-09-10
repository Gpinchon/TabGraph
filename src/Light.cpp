/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/10 21:42:11 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/10 23:43:41 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Light.hpp"
#include "Engine.hpp"

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

LightType	Light::type()
{
	return (Point);
}

DirectionnalLight::DirectionnalLight(const std::string &name) : Light(name) {
}

DirectionnalLight	*DirectionnalLight::create(const std::string &name, VEC3 color, VEC3 position, float power)
{
	auto	light = new DirectionnalLight(name);
	light->color() = color;
	light->position() = position;
	light->power() = power;
	Engine::add(*light);
	return (light);
}

LightType	DirectionnalLight::type()
{
	return (Directionnal);
}