/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CubeMesh.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/19 22:57:35 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/19 23:05:31 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.hpp"

namespace CubeMesh
{
	Mesh	*create(const std::string &name, VEC3 size);
};