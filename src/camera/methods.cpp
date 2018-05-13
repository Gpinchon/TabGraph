/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 16:30:02 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/05 14:29:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Window.hpp"

Camera::Camera(const std::string &name) : Node(name)
{

}

Camera		&Camera::create(const std::string &name, float fov)
{
	Camera	*camera;

	camera = new Camera(name);
	camera->fov = fov;
	Engine::add(*camera);
	return (*camera);
}

void	Camera::orbite(float phi, float theta, float radius)
{
	VEC3		target_position;
	VEC3		new_position;

	if (target)
		target_position = target->position();
	else
		target_position = new_vec3(0, 0, 0);
	new_position.x = target_position.x + radius * sin(phi) * cos(theta);
	new_position.z = target_position.z + radius * sin(phi) * sin(theta);
	new_position.y = target_position.y + radius * cos(phi);
	position() = new_position;
}

void	Camera::update()
{
	VEC3		target_position;
	VEC2		size;

	target_position = target ? target->position() : new_vec3(0, 0, 0);
	view = mat4_lookat(position(), target_position, UP);
	size = Window::size();
	projection = mat4_perspective(fov,
		size.x / size.y, 0.1, 1000);
}
