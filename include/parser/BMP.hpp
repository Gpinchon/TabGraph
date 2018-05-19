/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BMP.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:16:59 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 20:21:31 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Texture.hpp"

/*
** .BMP parsing interface
*/
class	BMP : public Texture
{
public:
	static Texture	*parse(const std::string &texture_name, const std::string &imagepath);
	static void		save(const Texture &, const std::string &);
private:
	virtual void abstract() = 0;
};