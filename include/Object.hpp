/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Object.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/07/30 19:13:58 by anonymous         #+#    #+#             */
/*   Updated: 2018/07/30 19:44:54 by anonymous        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
