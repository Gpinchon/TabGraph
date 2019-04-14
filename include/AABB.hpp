/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AABB.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2019/04/13 19:43:23 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "BoundingElement.hpp"

class AABB : public BoundingElement {
public:
    AABB()
        : BoundingElement(){};
    bool collides(const BoundingElement&) { return (false); };
};
