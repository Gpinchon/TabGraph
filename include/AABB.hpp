/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AABB.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 20:20:56 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "BoundingElement.hpp"

class AABB : public BoundingElement {
public:
    AABB()
        : BoundingElement() {};
    bool collides(const BoundingElement&) { return (false); };
};
