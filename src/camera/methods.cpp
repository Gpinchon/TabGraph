/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:30:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/20 01:34:50 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Window.hpp"

Camera::Camera(const std::string &name) : Node(name)
{

}

Camera		*Camera::create(const std::string &name, float fov)
{
	Camera	*camera;

	camera = new Camera(name);
	camera->fov = fov;
	Engine::add(*camera);
	return (camera);
}

void	Camera::orbite(float phi, float theta, float radius)
{
	VEC3		target_position {new_vec3(0, 0, 0)};
	VEC3		new_position {position()};

	if (target != nullptr) {
		target_position = target->position();
	}
	else {
		target_position = new_vec3(0, 0, 0);
	}
	new_position.x = target_position.x + radius * sin(phi) * cos(theta);
	new_position.z = target_position.z + radius * sin(phi) * sin(theta);
	new_position.y = target_position.y + radius * cos(phi);
	position() = new_position;
}

void	Camera::update()
{
	VEC3		target_position {target != nullptr ? target->position() : new_vec3(0, 0, 0)};
	VEC2		size {Window::size()};

	view = mat4_lookat(position(), target_position, UP);
	projection = mat4_perspective(fov,
		size.x / size.y, 0.1, 1000);
}
