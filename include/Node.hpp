/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/19 18:05:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
	MAT4			&transform();
	MAT4			&translate();
	MAT4			&rotate();
	MAT4			&scale();
	VEC3			&position();
	VEC3			&rotation();
	VEC3			&scaling();
	VEC3			&up();
	void			add_child(Node &child);
	void			set_parent(Node &parent);
	Node				*parent{nullptr};
	std::vector<Node *> children;
	BoundingElement		*bounding_element{nullptr};
protected :
	VEC3		_position{0, 0, 0};
	VEC3		_rotation{0, 0, 0};
	VEC3		_scaling{1, 1, 1};
	VEC3		_up{0, 1, 0};
	MAT4		_transform{mat4_identity()};
	MAT4		_translate{0};
	MAT4		_rotate{0};
	MAT4		_scale{mat4_identity()};
	Node(const std::string &name);
};
