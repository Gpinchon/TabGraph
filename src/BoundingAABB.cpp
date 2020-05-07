/*
* @Author: gpi
* @Date:   2019-05-15 13:53:32
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-15 13:54:23
*/

#include "BoundingAABB.hpp"
#include "Ray.hpp"

BoundingAABB::BoundingAABB(glm::vec3 min, glm::vec3 max)
    : BoundingElement(Type::AABB), _min(min), _max(max)
{
}

std::shared_ptr<BoundingAABB> BoundingAABB::Create(glm::vec3 min, glm::vec3 max)
{
    return std::shared_ptr<BoundingAABB> (new BoundingAABB(min, max));
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

Intersection BoundingAABB::IntersectAABB(const std::shared_ptr<BoundingAABB> &other) const
{
    auto distance1(other->Min()  - Max());
    auto distance2(Min()  - other->Max());
    auto distance(glm::max(distance1, distance2));
    auto maxDistance(glm::compMax(distance));
    return Intersection(maxDistance < 0, maxDistance);
}

Intersection BoundingAABB::IntersectRay(const Ray &ray) const
{
    /*glm::vec3 min = (Min() - ray.origin) / ray.direction;
    glm::vec3 max = (Max() - ray.origin) / ray.direction;
    float tmin = glm::compMin(min);
    float tmax = glm::compMin(max);
    if (tmin > tmax)
        return intersection;*/
    float tmin = (Min().x - ray.origin.x) / ray.direction.x;
    float tmax = (Max().x - ray.origin.x) / ray.direction.x;

    if (tmin > tmax) std::swap(tmin, tmax);

    float tymin = (Min().y - ray.origin.y) / ray.direction.y;
    float tymax = (Max().y - ray.origin.y) / ray.direction.y;

    if (tymin > tymax) std::swap(tymin, tymax);

    if ((tmin > tymax) || (tymin > tmax)) 
        return Intersection(false, tmin);

    if (tymin > tmin) 
        tmin = tymin;

    if (tymax < tmax) 
        tmax = tymax;

    float tzmin = (Min().z - ray.origin.z) / ray.direction.z;
    float tzmax = (Max().z - ray.origin.z) / ray.direction.z;

    if (tzmin > tzmax) std::swap(tzmin, tzmax);

    if ((tmin > tzmax) || (tzmin > tmax)) 
        return Intersection(false, tmin);
    if (tzmin > tmin) 
        tmin = tzmin;
    if (tzmax < tmax) 
        tmax = tzmax;

    //intersection.intersects = true;
    //intersection.distance = tmin;
    //intersection.position = ray.direction * intersection.distance + ray.origin;
    //auto c = (Min() + Max()) * 0.5f;
    //auto d = (Min() - Max()) * 0.5f;
    //auto p = intersection.position - c;
    //auto bias = 1.000001;
    //intersection.normal = glm::vec3(p.x / abs(d.x) * bias,
    //    p.y / abs(d.y) * bias,
    //    p.z / abs(d.z) * bias);
    //intersection.normal = normalize(intersection.normal);

    return Intersection(true, tmin);
}