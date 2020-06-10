/*
* @Author: gpi
* @Date:   2019-05-15 13:53:32
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-05-29 13:05:58
*/

#include "Physics/BoundingAABB.hpp"
#include "Physics/IntersectFunctions.hpp"
#include "Physics/Ray.hpp"

BoundingAABB::BoundingAABB(glm::vec3 min, glm::vec3 max)
    : BoundingElement(Type::AABB)
    , _min(min)
    , _max(max)
{
}

std::shared_ptr<BoundingAABB> BoundingAABB::Create(glm::vec3 min, glm::vec3 max)
{
    return std::shared_ptr<BoundingAABB>(new BoundingAABB(min, max));
}

glm::vec3 BoundingAABB::Min() const
{
    return _min;
}

void BoundingAABB::SetMin(glm::vec3 min)
{
    _min = min;
}

glm::vec3 BoundingAABB::Max() const
{
    return _max;
}

void BoundingAABB::SetMax(glm::vec3 max)
{
    _max = max;
}

#include <glm/gtx/component_wise.hpp>

Intersection BoundingAABB::IntersectAABB(const std::shared_ptr<BoundingAABB>& other) const
{
    return IntersectFunction(*this, *other);
}

static inline Intersection CreateAABBRayIntersection(const bool intersects, const float distance, const BoundingAABB* aabb, const Ray& ray)
{
    auto position = ray.direction * distance + ray.origin;
    auto c = (aabb->Min() + aabb->Max()) * 0.5f;
    auto d = (aabb->Min() - aabb->Max()) * 0.5f;
    auto p = position - c;
    auto bias = 1.000001;
    auto normal = glm::vec3(p.x / abs(d.x) * bias,
        p.y / abs(d.y) * bias,
        p.z / abs(d.z) * bias);
    normal = normalize(normal);
    return Intersection(intersects, distance, normal, position);
}

Intersection BoundingAABB::IntersectRay(const Ray& ray) const
{
    /*glm::vec3 min = (Min() - ray.origin) / ray.direction;
    glm::vec3 max = (Max() - ray.origin) / ray.direction;
    float tmin = glm::compMin(min);
    float tmax = glm::compMin(max);
    if (tmin > tmax)
        return intersection;*/
    float tmin = (Min().x - ray.origin.x) / ray.direction.x;
    float tmax = (Max().x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax)
        std::swap(tmin, tmax);

    float tymin = (Min().y - ray.origin.y) / ray.direction.y;
    float tymax = (Max().y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax)
        std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax))
        return CreateAABBRayIntersection(false, tmin, this, ray);

    if (tymin > tmin)
        tmin = tymin;

    if (tymax < tmax)
        tmax = tymax;

    float tzmin = (Min().z - ray.origin.z) / ray.direction.z;
    float tzmax = (Max().z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax)
        std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax))
        return CreateAABBRayIntersection(false, tmin, this, ray);
    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    return CreateAABBRayIntersection(true, tmin, this, ray);
}