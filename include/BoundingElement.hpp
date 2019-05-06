/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:28:57
*/

#pragma once

#include "GLIncludes.hpp"
#include <limits>

class BoundingElement {
public:
    BoundingElement() = default;
    virtual bool collides(const BoundingElement&) = 0;
    VEC3 min{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    VEC3 max{ std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };
    VEC3 center{ 0, 0, 0 };
};
