/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OBJ.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:17:05 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/13 00:17:05 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.hpp"

/*
** .OBJ parsing interface
*/
class	OBJ : public Mesh
{
public:
	static Mesh	*parse(const std::string &, const std::string &);
private:
	virtual abstract() = 0;
};
