/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MTLLIB.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 20:10:57 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/09 18:56:35 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Material.hpp"

/*
** .MTLLIB parsing interface
*/
class	MTLLIB : public Material
{
public:
	static bool parse(const std::string &);
private:
	virtual void abstract() = 0;
};

class	PBRMTLLIB : public Material
{
public:
	static bool	parse(const std::string &);
private:
	virtual void abstract() = 0;
};
