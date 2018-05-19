/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OBJ.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:17:05 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 20:21:11 by gpinchon         ###   ########.fr       */
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
	virtual void abstract() = 0;
};
