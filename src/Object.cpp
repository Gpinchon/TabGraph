/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:17:06 by anonymous         #+#    #+#             */
/*   Updated: 2019/02/17 22:11:04 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Object.hpp"

std::vector<std::shared_ptr<Object>> Object::_objects;

Object::Object()
{
    auto objectName = std::string("object_") + std::to_string(_objects.size());
    auto object = std::shared_ptr<Object>(new Object(objectName));
    _objects.push_back(object);
}

Object::Object(const std::string& name)
{
    set_name(name);
}

const std::string& Object::name()
{
    return (_name);
}

void Object::set_name(const std::string& name)
{
    _name = name;
}