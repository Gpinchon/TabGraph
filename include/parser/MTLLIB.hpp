/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MTLLIB.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 20:10:57 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/24 11:43:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Material.hpp"

/*
** .MTLLIB parsing interface
*/
namespace MTLLIB {
bool parse(const std::string& path);
};
