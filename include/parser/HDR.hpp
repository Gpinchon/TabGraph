/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HDR.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/06/03 18:07:46 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/07 23:01:46 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"

/*
** .HDR parsing interface
*/
class HDR : public Texture
{
public :
	static Texture	*parse(const std::string &texture_name, const std::string &imagepath);
private :
	virtual void abstract() = 0;
/*	HDR();
	~HDR();*/
};