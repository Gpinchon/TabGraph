#pragma once

#include "BoundingElement.hpp"
#include "Intersection.hpp"
#include "Ray.hpp"

class BoundingSphere : public BoundingElement
{
public:
	static std::shared_ptr<BoundingSphere> Create(const glm::vec3 &center, float radius);
	BoundingSphere(const glm::vec3 &center, float radius);
	glm::vec3 GetCenter() const;
	void SetCenter(glm::vec3 center);
	float GetRadius() const;
	void SetRadius(float radius);
	virtual Intersection IntersectSphere(const std::shared_ptr<BoundingSphere> &other) const override;
	virtual Intersection IntersectRay(const Ray &ray) const override;

private:
	glm::vec3 _center { 0 };
	float _radius { 0 };
};
