/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AABB.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 21:21:23 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "BoundingElement.hpp"

struct	AABB : public BoundingElement
{
	AABB() : BoundingElement() {};
	bool	collides(const BoundingElement &) { return (false); };
};
