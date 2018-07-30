/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/30 19:41:33 by anonymous        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "vml.h"
#include "Object.hpp"
#include <string>
#include <vector>

class BoundingElement;

class Node : public Object
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
	Node				*parent{nullptr};
	std::vector<Node *> children;
	BoundingElement		*bounding_element{nullptr};
protected :
	t_transform	_transform;
	Node(const std::string &name);
};
