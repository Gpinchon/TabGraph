/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OBJ.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:17:05 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 19:13:49 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>

class Mesh;

/*
** .OBJ parsing interface
*/
namespace	OBJ
{
	std::shared_ptr<Mesh>	parse(const std::string &, const std::string &);
};
