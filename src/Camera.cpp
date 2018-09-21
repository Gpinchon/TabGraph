/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:30:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:03:18 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Window.hpp"
#include <iostream>

std::vector<std::shared_ptr<Camera>>	Camera::_cameras;
std::weak_ptr<Camera>					Camera::_current;

Camera::Camera(const std::string &name, float ifov, CameraProjection proj) : Node(name)
{
	_fov = ifov;
	_projection_type = proj;
}

std::shared_ptr<Camera>	Camera::create(const std::string &name, float ifov, CameraProjection proj)
{
	auto	camera = std::shared_ptr<Camera>(new Camera(name, ifov, proj));
	_nodes.push_back(std::static_pointer_cast<Node>(camera));
	_cameras.push_back(camera);
	return (camera);
}

std::shared_ptr<Camera>	Camera::get(unsigned index)
{
	if (index >= _cameras.size())
		return (nullptr);
	return (_cameras.at(index));
}

std::shared_ptr<Camera>	Camera::get_by_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto n : _cameras) {
		if (h == n->id())
			return (n);
	}
	return (nullptr);
}

std::shared_ptr<Camera>	Camera::current()
{
	return (_current.lock());
}

void	Camera::set_current(std::shared_ptr<Camera> camera)
{
	_current = camera;
}

void	Camera::fixed_update()
{
	//Node::update();
	if (nullptr != target())
		transform() = mat4_lookat(position(), target()->position(), UP);
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
	return (transform());
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

std::shared_ptr<OrbitCamera>	OrbitCamera::create(const std::string &iname, float ifov, float phi, float theta, float radius)
{
	auto	camera = std::shared_ptr<OrbitCamera>(new OrbitCamera(iname, ifov, phi, theta, radius));
	_nodes.push_back(std::static_pointer_cast<Node>(camera));
	_cameras.push_back(std::static_pointer_cast<Camera>(camera));
	return (camera);
}

void	OrbitCamera::orbite(float phi, float theta, float radius)
{
	VEC3		target_position {0, 0, 0};
	VEC3		new_position  = position();

	_phi = phi;
	_theta = theta;
	_radius = radius;
	if (target() != nullptr) {
		target_position = target()->position();
	}
	else {
		target_position = new_vec3(0, 0, 0);
	}
	new_position.x = target_position.x + _radius * sin(_phi) * cos(_theta);
	new_position.z = target_position.z + _radius * sin(_phi) * sin(_theta);
	new_position.y = target_position.y + _radius * cos(_phi);
	position() = new_position;
}
