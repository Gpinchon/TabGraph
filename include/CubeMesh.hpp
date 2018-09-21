/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CubeMesh.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/09/19 22:57:35 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 15:36:31 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.hpp"

namespace CubeMesh
{
	std::shared_ptr<Mesh>	create(const std::string &name, VEC3 size);
};