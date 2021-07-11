/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:09
*/

#pragma once

#include "BoundingElement.hpp"

/** @brief BoundingElement class

	This class provides the Axis Aligned Bounding Box bounding element
	@author: G. Pinchon
*/
class BoundingAABB : public BoundingElement {
public:
    BoundingAABB(glm::vec3 min, glm::vec3 max);
    virtual glm::mat3 LocalInertiaTensor(const float& mass) const override;
    virtual Intersection IntersectRay(const Ray& ray) const override;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform) const override;
    virtual ProjectionInterval Project(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    //virtual CollisionEdge GetBestEdge(glm::vec3 axis, const glm::mat4 &transform = glm::mat4(1.f)) override;
    glm::vec3 Min() const;
    void SetMin(glm::vec3 min);
    glm::vec3 Max() const;
    void SetMax(glm::vec3 max);

private:
    virtual std::shared_ptr<Component> _Clone() override
    {
        return std::make_shared<BoundingAABB>(*this);
    }
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
