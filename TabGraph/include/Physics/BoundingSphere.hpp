#pragma once

#include "BoundingElement.hpp"

class BoundingSphere : public BoundingElement {
public:
    BoundingSphere(const glm::vec3& center, float radius);
    virtual glm::mat3 LocalInertiaTensor(const float& mass) const override;
    virtual Intersection IntersectRay(const Ray& ray) const override;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform) const override;
    virtual ProjectionInterval Project(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    static std::shared_ptr<BoundingSphere> Create(const glm::vec3& center, float radius);
    glm::vec3 GetCenter() const;
    void SetCenter(glm::vec3 center);
    float GetRadius() const;
    void SetRadius(float radius);

private:
    glm::vec3 _center { 0 };
    float _radius { 0 };
};
