/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 23:01:39 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "vml.h"
//#include "Engine.hpp"
#include <string>
#include <vector>

class BoundingElement;

class Node
{
public:
	static Node *create(const std::string &name, VEC3 position, VEC3 rotation, VEC3 scale);
	static Node	*get_by_name(const std::string &);
	virtual void	physics_update();
	virtual void	fixed_update() {};
	virtual void	update() {};
	virtual void	render() {};
	MAT4			&mat4_transform();
	MAT4			&mat4_translation();
	MAT4			&mat4_rotation();
	MAT4			&mat4_scale();
	VEC3			&position();
	VEC3			&rotation();
	VEC3			&scale();
	VEC3			&up();
	void			add_child(Node &child);
	void			set_parent(Node &parent);
	void			set_name(const std::string &);
	const std::string	&name();
	Node				*parent{nullptr};
	std::vector<Node *> children;
	BoundingElement		*bounding_element{nullptr};
protected :
	size_t		_id;
	std::string	_name;
	t_transform	_transform;
	Node(const std::string &name);
};
