/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:17:06 by anonymous         #+#    #+#             */
/*   Updated: 2018/09/20 16:52:14 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Object.hpp"

Object::Object(const std::string &name)
{
	set_name(name);
}

const std::string	&Object::name()
{
	return (_name);
}

size_t				Object::id()
{
	return (_id);
}

void				Object::set_name(const std::string &name)
{
	_name = name;
	std::hash<std::string> hash_fn;
	_id = hash_fn(name);
}