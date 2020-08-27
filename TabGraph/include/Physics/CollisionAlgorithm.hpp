#pragma once

#include <memory>

class Collision;
class RigidBody;

class CollisionAlgorithm {
public:
    virtual bool Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out) = 0;
};