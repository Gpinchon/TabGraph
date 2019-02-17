/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:13:58 by anonymous         #+#    #+#             */
/*   Updated: 2019/02/17 22:06:36 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include <vector>
#include <memory>
#include <string>
/*#include <iostream>
#include <memory>
#include <string>*/

/*
** Never allocate on the stack, always of the heap !!!
** ALWAYS STORE IN A SHARED_PTR !!!
*/

class Object : public std::enable_shared_from_this<Object>
{
public:
	const std::string	&name();
	void				set_name(const std::string &name);
protected :
	Object();
	Object(const std::string &name);
private:
	std::string	_name;
	static std::vector<std::shared_ptr<Object>> _objects;
};
