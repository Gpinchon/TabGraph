#include "Physics/BoundingSphere.hpp"
#include "Physics/Ray.hpp"
#include "Physics/IntersectFunctions.hpp"
#include "Callback.hpp"

#include <iostream>
BoundingSphere::BoundingSphere(const glm::vec3 &center, float radius) : BoundingElement(Sphere), _center(center), _radius(radius)
{
}

std::shared_ptr<BoundingSphere> BoundingSphere::Create(const glm::vec3 &center, float radius)
{
    return std::shared_ptr<BoundingSphere>(new BoundingSphere(center, radius));
}

glm::vec3 BoundingSphere::GetCenter() const
{
	return _center;
}

void BoundingSphere::SetCenter(glm::vec3 center)
{
	_center = center;
}

float BoundingSphere::GetRadius() const
{
	return _radius;
}

void BoundingSphere::SetRadius(float radius)
{
	_radius = radius;
}

Intersection BoundingSphere::IntersectSphere(const std::shared_ptr<BoundingSphere> &other) const
{
	return IntersectFunction(*this, *other);
}

void BoundingSphere::Transform(const glm::vec3 &translation, const glm::quat &/*rotation*/, const glm::vec3 &/*scale*/)
{
    SetCenter(translation);
}

glm::vec3    sphere_normal(glm::vec3 position, glm::vec3 intersectPosition, float radius)
{
    return normalize((position - intersectPosition) / radius);
}

bool        intersect_test(float t0, float t1)
{
    return (t0 > 0.f || t1 > 0.f);
}

bool        solve_quadratic(float a, float b, float c, float *t)
{
    float   discrim;
    float   q;

    discrim = (b * b - 4.f * a * c);
    if (discrim < 0)
        return false;
    q = -.5f * (b < 0 ? (b - sqrtf(discrim)) : (b + sqrtf(discrim)));
    auto t0 = q / a;
    auto t1 = c / q;
    if (t0 > t1)
    {
        q = t0;
        t0 = t1;
        t1 = q;
    }
    *t = glm::min(t0, t1);
    return intersect_test(t0, t1);
}

Intersection SphereIntersection(const Ray ray, glm::vec3 position, float /*radius*/, float radius2)
{
    glm::vec3       eye;
    float			distance(0.f);

    eye = ray.origin - position;
    if (!(solve_quadratic(
                glm::dot(ray.direction, ray.direction),
                glm::dot(eye, ray.direction) * 2.0,
                glm::dot(eye, eye) - radius2,
                &distance)))
        return Intersection(false, distance);
    //glm::vec3 position = ray.origin + ray.direction * distance;
    //glm::vec3 normal = sphere_normal(position, position, radius);
    return (Intersection(true, distance));
}

Intersection BoundingSphere::IntersectRay(const Ray &ray) const
{
	return SphereIntersection(ray, GetCenter(), GetRadius(), GetRadius() * GetRadius());
}