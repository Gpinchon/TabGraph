#pragma once

#include "BoundingElement.hpp"
#include <memory>

class Mesh;

class BoundingMesh : public BoundingElement {
public:
    BoundingMesh(const std::shared_ptr<Mesh>& mesh);
    static std::shared_ptr<BoundingMesh> Create(const std::shared_ptr<Mesh>& mesh);
    std::shared_ptr<Mesh> GetMesh() const;
    void SetMesh(const std::shared_ptr<Mesh>& mesh);
    virtual glm::mat3 LocalInertiaTensor(const float& mass) const override;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual ProjectionInterval Project(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    //virtual glm::vec3 GetSupportPoint(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    //virtual CollisionEdge GetBestEdge(glm::vec3 axis, const glm::mat4 &transform = glm::mat4(1.f)) override;

private:
    virtual std::shared_ptr<Component> _Clone() const override {
        return tools::make_shared<BoundingMesh>(*this);
    }
};