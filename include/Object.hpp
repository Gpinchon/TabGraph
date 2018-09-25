/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:13:58 by anonymous         #+#    #+#             */
/*   Updated: 2018/09/25 18:44:24 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include <iostream>
#include <memory>
#include <string>

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

class Object : public std::enable_shared_from_this<Object>
{
public:
	const std::string	&name();
	size_t				id();
	void				set_name(const std::string &name);
protected :
	Object(const std::string &name);
private:
	size_t		_id{0};
	std::string	_name;
};
