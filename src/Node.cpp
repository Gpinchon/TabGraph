/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:10:01 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 11:36:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Node.hpp"

std::vector<std::shared_ptr<Node>>	Node::_nodes;

Node::Node(const std::string &name) : Object(name) /*parent(nullptr),  bounding_element(nullptr), _transform(new_transform(new_vec3(0, 0, 0), new_vec3(0, 0, 0), new_vec3(1, 1, 1), UP))*/
{
}

std::shared_ptr<Node>	Node::create(const std::string &name, VEC3 position, VEC3 rotation, VEC3 scale)
{
	auto	t = std::shared_ptr<Node>(new Node(name));
	t->_position = position;
	t->_rotation = rotation;
	t->_scaling = scale;
	//t->_transform = new_transform(position, rotation, scale, UP);
	t->update();
	_nodes.push_back(t);
	return (t);
}

std::shared_ptr<Node>	Node::get_by_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto n : _nodes) {
		if (h == n->id())
			return (n);
	}
	return (nullptr);
}

std::shared_ptr<Node>	Node::get(unsigned index)
{
	if (index >= _nodes.size())
		return (nullptr);
	return (_nodes.at(index));
}


void	Node::physics_update()
{
	_translate = mat4_translate(_position);
	_rotate = mat4_rotation(_rotation);
	_scale = mat4_scale(_scaling);
	_transform = mat4_combine(_translate, _rotate, _scale);
	//transform_update(&_transform);
	if (parent() != nullptr) {
		_transform = mat4_mult_mat4(parent()->transform(), _transform);
		//transform_set_parent(&_transform, &parent()->_transform);
	}
}

void	Node::add_child(std::shared_ptr<Node> childNode)
{
	if (childNode == shared_from_this()) {
		return ;
	}
	_children.push_back(childNode);
	childNode->set_parent(shared_from_this());
}

std::shared_ptr<Node>	Node::target()
{
	return (_target.lock());
}

void					Node::set_target(std::shared_ptr<Node> tgt)
{
	_target = tgt;
}

std::shared_ptr<Node>	Node::parent()
{
	return (_parent.lock());
}

/*
** /!\ BEWARE OF THE BIG BAD LOOP !!! /!\
*/
void					Node::set_parent(std::shared_ptr<Node> prt)
{
	if (prt == shared_from_this()) {
		return ;
	}
	_parent = prt;
	prt->add_child(shared_from_this());
}


VEC3	&Node::up()
{
	return (_up);
}

VEC3	&Node::position()
{
	return (_position);
}

VEC3	&Node::rotation()
{
	return (_rotation);
}

VEC3	&Node::scaling()
{
	return (_scaling);
}

MAT4	&Node::transform()
{
	return (_transform);
}

MAT4	&Node::translate()
{
	return (_translate);
}

MAT4	&Node::rotate()
{
	return (_rotate);
}

MAT4	&Node::scale()
{
	return (_scale);
}
