#include "BoundingPlane.hpp"
#include "BoundingSphere.hpp"
#include "Ray.hpp"

BoundingPlane::BoundingPlane(glm::vec3 normal, float distance) : BoundingElement(Plane), _normal(normal), _distance(distance)
{

}

BoundingPlane BoundingPlane::Normalized() const
{
	auto magnitude(glm::length(GetNormal()));
	return BoundingPlane(GetNormal() / magnitude, GetDistance() / magnitude);
}

Intersection BoundingPlane::IntersectSphere(const std::shared_ptr<BoundingSphere> &other) const
{
	const auto distanceFromSphereCenter(glm::dot(GetNormal(), other->GetCenter()) + GetDistance());
	const auto distanceFromSphere(distanceFromSphereCenter - other->GetRadius());
	return Intersection(distanceFromSphere < 0, distanceFromSphere);
}

Intersection BoundingPlane::IntersectRay(const Ray &ray) const
{
	auto denom = glm::dot(GetNormal(), ray.direction);
	float t(0.f);
	if (denom > 1e-6) {
		auto p0(GetNormal() * GetDistance());
		auto p0l0 = p0 - ray.origin;
		t = glm::dot(p0l0, GetNormal()) / denom;
	}
	return Intersection(t >= 0, t);
}

glm::vec3 BoundingPlane::GetNormal() const
{
	return _normal;
}

void BoundingPlane::SetNormal(const glm::vec3 &normal)
{
	_normal = normal;
}

float BoundingPlane::GetDistance() const
{
	return _distance;
}

void BoundingPlane::SetDistance(float distance)
{
	_distance = distance;
}
