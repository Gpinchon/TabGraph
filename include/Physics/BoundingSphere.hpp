#pragma once

#include "BoundingElement.hpp"

class BoundingSphere : public BoundingElement
{
public:
	BoundingSphere(const glm::vec3 &center, float radius);
	virtual Intersection IntersectRay(const Ray &ray) const override;
	virtual Intersection IntersectSphere(const std::shared_ptr<BoundingSphere> &other) const override;
	virtual void Transform(const glm::vec3 &translation, const glm::quat &rotation, const glm::vec3 &scale) override;
	static std::shared_ptr<BoundingSphere> Create(const glm::vec3 &center, float radius);
	glm::vec3 GetCenter() const;
	void SetCenter(glm::vec3 center);
	float GetRadius() const;
	void SetRadius(float radius);
	

private:
	glm::vec3 _center { 0 };
	float _radius { 0 };
};
