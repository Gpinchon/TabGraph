/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-27 17:16:56
*/

#pragma once

#include <glm/glm.hpp>
#include <limits>

class BoundingElement {
public:
    BoundingElement() = default;
    virtual bool collides(const BoundingElement&) = 0;
    glm::vec3 min { std::numeric_limits<float>::max() };
    glm::vec3 max { std::numeric_limits<float>::lowest() };
    glm::vec3 center { 0, 0, 0 };
};
