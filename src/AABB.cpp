/*
* @Author: gpi
* @Date:   2019-05-15 13:53:32
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-15 13:54:23
*/

#include "AABB.hpp"

AABB::AABB()
    : BoundingElement()
{
}

bool AABB::collides(const BoundingElement&)
{
    return (false);
}