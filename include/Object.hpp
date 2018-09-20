/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:13:58 by anonymous         #+#    #+#             */
/*   Updated: 2018/09/20 16:52:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include <string>

class Object
{
public:
	Object() = default;
	Object(const std::string &name);
	const std::string	&name();
	size_t				id();
	void				set_name(const std::string &name);
private:
	size_t		_id{0};
	std::string	_name;
};
