#pragma once
#include "Physics/CollisionAlgorithm.hpp"

class CollisionAlgorithmSAT : public CollisionAlgorithm {
public:
    virtual bool Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out) override;
};