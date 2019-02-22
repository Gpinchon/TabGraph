/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BMP.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:16:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 17:21:33 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>

class Texture;

/*
** .BMP parsing interface
*/
namespace BMP {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
void save(std::shared_ptr<Texture>, const std::string&);
};