#pragma once

#include "BoundingElement.hpp"

class BoundingPlane : public BoundingElement
{
public:
	BoundingPlane(glm::vec3 normal, float distance);
	virtual Intersection IntersectRay(const Ray &ray) const override;
	virtual Intersection IntersectSphere(const std::shared_ptr<BoundingSphere> &other) const override;
	virtual void Transform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale) override;
	static std::shared_ptr<BoundingPlane> Create(const glm::vec3 &normal, float distance);
	BoundingPlane Normalized() const;
	glm::vec3 GetNormal() const;
	void SetNormal(const glm::vec3 &normal);
	float GetDistance() const;
	void SetDistance(float);

private:
	glm::vec3 _normal { 0 };
	float _distance { 0 };
};
