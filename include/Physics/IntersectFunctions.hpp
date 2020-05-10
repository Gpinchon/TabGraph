#include "BoundingSphere.hpp"
#include "BoundingAABB.hpp"
#include "BoundingPlane.hpp"
#include "BoundingMesh.hpp"
#include "BoundingBox.hpp"
#include <glm/gtx/component_wise.hpp>

inline Intersection IntersectFunction(const BoundingSphere &a,const BoundingSphere &b)
{
	float radiusDistance = a.GetRadius() + b.GetRadius();
	float centerDistance = glm::distance(a.GetCenter(), b.GetCenter());
	float distance = centerDistance - radiusDistance;    
	return Intersection(centerDistance < radiusDistance, distance);
}


inline Intersection IntersectFunction(const BoundingAABB &a, const BoundingAABB &b)
{
	auto distance1(b.Min() - a.Max());
    auto distance2(a.Min() - b.Max());
    auto distance(glm::max(distance1, distance2));
    auto maxDistance(glm::compMax(distance));
    return Intersection(maxDistance < 0, maxDistance);
}