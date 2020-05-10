/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-15 13:55:26
*/

#pragma once

#include "BoundingElement.hpp"

/** @brief BoundingElement class

	This class provides the Axis Aligned Bounding Box bounding element
	@author: G. Pinchon
*/
class BoundingAABB : public BoundingElement {
public:
    static std::shared_ptr<BoundingAABB> Create(glm::vec3 min, glm::vec3 max);
    virtual Intersection IntersectRay(const Ray &ray) const override;
    virtual Intersection IntersectAABB(const std::shared_ptr<BoundingAABB> &other) const override;
    BoundingAABB(glm::vec3 min, glm::vec3 max);
    glm::vec3 Min() const;
    void SetMin(glm::vec3 min);
    glm::vec3 Max() const;
    void SetMax(glm::vec3 max);

private:
    glm::vec3 _min { 0 };
    glm::vec3 _max { 0 };
};

/*
template <>
inline Intersection BoundingElement::Intersect(const BoundingAABB &a, const BoundingAABB &b)
{
    return a.IntersectAABB(b);
}
*/
