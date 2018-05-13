/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MTLLIB.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 20:10:57 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/13 20:10:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "PBRMaterial.hpp"

/*
** .MTLLIB parsing interface
*/
class	MTLLIB : public Material
{
public:
	static bool parse(const std::string &);
private:
	virtual     abstract() = 0;
};

class	PBRMTLLIB : public PBRMaterial
{
public:
	static bool	parse(const std::string &);
private:
	virtual     abstract() = 0;
};
