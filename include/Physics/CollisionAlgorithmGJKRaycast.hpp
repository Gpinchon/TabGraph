#pragma once
#include "Physics/CollisionAlgorithm.hpp"
#include <glm/glm.hpp>

class CollisionAlgorithmGJKRaycast : public CollisionAlgorithm {
public:
    struct IntersectionResult {
        auto Normal() const { return _normal; }
        void SetNormal(const glm::vec3& normal) { _normal = normal; }
        auto HitPointA() const { return _hitPointA; };
        void SetHitPointA(const glm::vec3& hitPointA) { _hitPointA = hitPointA; }
        auto HitPointB() const { return _hitPointB; };
        void SetHitPointB(const glm::vec3& hitPointB) { _hitPointB = hitPointB; }
        auto HitPoint() const { return (HitPointA() + HitPointB()) / 2.f; }
        auto PenetrationDepth() const
        { /*return dot(HitPointB() - HitPointA(), -Normal());*/
            return _penetrationDepth;
        }
        void SetPenetrationDepth(float penetrationDepth) { _penetrationDepth = penetrationDepth; }
        auto Fraction() const { return _fraction; }
        void SetFraction(float fraction) { _fraction = fraction; }
        auto AllowedPenetration() const { return _allowedPenetration; }
        void SetAllowedPenetration(float allowedPenetration) { _allowedPenetration = allowedPenetration; }

    private:
        float _fraction { 1 };
        glm::vec3 _normal { 0 };
        glm::vec3 _hitPointA { std::numeric_limits<float>::infinity() };
        glm::vec3 _hitPointB { std::numeric_limits<float>::infinity() };
        float _allowedPenetration { 0.f };
        float _penetrationDepth { 0.f };
    };
    virtual bool Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out) override;
    float AllowedPenetration() const;
    void SetAllowedPenetration(float);
    float GetFraction() const;

private:
    float _allowedPenetration { 0.f };
    IntersectionResult _intersectionResult;
};