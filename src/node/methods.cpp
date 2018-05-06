/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:10:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/05 14:22:37 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

Node::Node(const std::string &name) : parent(nullptr), _transform(new_transform(new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1), UP))
{
	std::cout << "Node::Node\n";
	set_name(name);
	std::cout << name << std::endl;
}

void	Node::set_name(const std::string &name)
{
	_name = name;
	std::hash<std::string> hash_fn;
	_id = hash_fn(name);
}

Node	&Node::create(const std::string &name, VEC3 position, VEC3 rotation, VEC3 scale)
{
	Node	*t;

	t = new Node(name);
	t->_transform = new_transform(position, rotation, scale, UP);
	t->update();
	Engine::add(*t);
	return (*t);
}

const std::string	&Node::name()
{
	return (_name);
}

void	Node::physics_update()
{
	transform_update(&_transform);
	if (parent)
		transform_set_parent(&_transform, &parent->_transform);
}

void	Node::add_child(Node &child)
{
	children.push_back(&child);
	child.parent = this;
}

VEC3	&Node::up()
{
	return (_transform.up);
}

VEC3	&Node::position()
{
	return (_transform.position);
}

VEC3	&Node::rotation()
{
	return (_transform.rotation);
}

VEC3	&Node::scale()
{
	return (_transform.scaling);
}

MAT4	&Node::mat4_transform()
{
	//_transform.transform = mat4_identity();
	return (_transform.transform);
}

MAT4	&Node::mat4_translation()
{
	return (_transform.translate);
}

MAT4	&Node::mat4_rotation()
{
	return (_transform.rotate);
}

MAT4	&Node::mat4_scale()
{
	return (_transform.scale);
}
