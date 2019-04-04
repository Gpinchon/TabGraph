/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HDR.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/03 18:07:46 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/22 19:36:41 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <memory>

class Texture;

/*
** .HDR parsing interface
*/
namespace HDR {
std::shared_ptr<Texture> parse(const std::string& texture_name, const std::string& imagepath);
};