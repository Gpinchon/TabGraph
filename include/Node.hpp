/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Node.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/20 21:48:27 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Object.hpp"
#include <string>
#include <vector>
#include <memory>

class BoundingElement;

class Node : public Object, std::enable_shared_from_this<Node>
{
public:
	static std::shared_ptr<Node>		create(const std::string &name, VEC3 position, VEC3 rotation, VEC3 scale);
	static std::shared_ptr<Node>		get_by_name(const std::string &);
	static std::shared_ptr<Node>		node(unsigned index);
	virtual void						physics_update();
	virtual void						fixed_update() {};
	virtual void						update() {};
	virtual void						render() {};
	MAT4								&transform();
	MAT4								&translate();
	MAT4								&rotate();
	MAT4								&scale();
	VEC3								&position();
	VEC3								&rotation();
	VEC3								&scaling();
	VEC3								&up();
	std::shared_ptr<Node>				target();
	void								set_target(std::shared_ptr<Node>);
	std::shared_ptr<Node>				parent();
	void								set_parent(std::shared_ptr<Node>);
	void								add_child(std::shared_ptr<Node> child);
	std::shared_ptr<Node>				child(unsigned index);
	BoundingElement						*bounding_element{nullptr};
protected :
	Node(const std::string &name);
	std::vector<std::weak_ptr<Node>>	_children;
	std::weak_ptr<Node>					_target;
	std::weak_ptr<Node>					_parent;
	VEC3								_position{0, 0, 0};
	VEC3								_rotation{0, 0, 0};
	VEC3								_scaling{1, 1, 1};
	VEC3								_up{0, 1, 0};
	MAT4								_transform{mat4_identity()};
	MAT4								_translate{0};
	MAT4								_rotate{0};
	MAT4								_scale{mat4_identity()};
	static std::vector<std::shared_ptr<Node>>	_nodes;
};
