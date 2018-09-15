/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:30:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/15 17:26:51 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include <iostream>

Camera::Camera(const std::string &name, float ifov, CameraProjection proj) : Node(name)
{
	_fov = ifov;
	_projection_type = proj;
}

Camera		*Camera::create(const std::string &name, float ifov, CameraProjection proj)
{
	auto	camera = new Camera(name, ifov, proj);
	Engine::add(*camera);
	return (camera);
}

void	Camera::fixed_update()
{
	//Node::update();
	if (nullptr != target)
		mat4_transform() = mat4_lookat(position(), target->position(), UP);
	if (_projection_type == PerspectiveCamera)
	{
		VEC2	size = Window::size();
		_projection = mat4_perspective(_fov, size.x / size.y, _znear, _zfar);
	}
	else
		_projection = mat4_orthographic(_frustum, _znear, _zfar);
}

MAT4	&Camera::view()
{
	return (mat4_transform());
}

MAT4	&Camera::projection()
{
	return (_projection);
}

FRUSTUM	&Camera::frustum()
{
	return (_frustum);
}

float	&Camera::fov()
{
	return (_fov);
}

OrbitCamera::OrbitCamera(const std::string &iname, float ifov, float phi, float theta, float radius) : Camera(iname, ifov)
{
	_phi = phi;
	_theta = theta;
	_radius = radius;
}

OrbitCamera	*OrbitCamera::create(const std::string &iname, float ifov, float phi, float theta, float radius)
{
	auto	camera = new OrbitCamera(iname, ifov, phi, theta, radius);
	Engine::add(*camera);
	return (camera);
}

void	OrbitCamera::orbite(float phi, float theta, float radius)
{
	VEC3		target_position {0, 0, 0};
	VEC3		new_position  = position();

	_phi = phi;
	_theta = theta;
	_radius = radius;
	if (target != nullptr) {
		target_position = target->position();
	}
	else {
		target_position = new_vec3(0, 0, 0);
	}
	new_position.x = target_position.x + _radius * sin(_phi) * cos(_theta);
	new_position.z = target_position.z + _radius * sin(_phi) * sin(_theta);
	new_position.y = target_position.y + _radius * cos(_phi);
	position() = new_position;
}
