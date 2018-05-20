/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:10:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/20 01:22:27 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Node.hpp"

Node::Node(const std::string &name) : parent(nullptr),  bounding_element(nullptr), _id(0), _transform(new_transform(new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1), UP))
{
	set_name(name);
}

void	Node::set_name(const std::string &name)
{
	_name = name;
	std::hash<std::string> hash_fn;
	_id = hash_fn(name);
}

Node	*Node::get_by_name(const std::string &name)
{
	int		i;
	size_t		h;
	Node	*m;

	i = 0;
	std::hash<std::string>	hash_fn;
	h = hash_fn(name);
	while ((m = Engine::node(i)) != nullptr)
	{
		if (h == m->_id) {
			return (m);
		}
		i++;
	}
	return (nullptr);
}

Node	*Node::create(const std::string &name, VEC3 position, VEC3 rotation, VEC3 scale)
{
	Node	*t;

	t = new Node(name);
	t->_transform = new_transform(position, rotation, scale, UP);
	t->update();
	Engine::add(*t);
	return (t);
}

const std::string	&Node::name()
{
	return (_name);
}

void	Node::physics_update()
{
	transform_update(&_transform);
	if (parent != nullptr) {
		transform_set_parent(&_transform, &parent->_transform);
	}
}

void	Node::add_child(Node &child)
{
	if (&child == this) {
		return ;
	}
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
