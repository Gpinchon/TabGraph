/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:25:13
*/

#pragma once

#include "BoundingElement.hpp"

class AABB : public BoundingElement {
public:
    AABB()
        : BoundingElement(){};
    bool collides(const BoundingElement&) { return (false); };
};
