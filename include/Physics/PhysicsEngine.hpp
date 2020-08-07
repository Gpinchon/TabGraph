#pragma once

#include "Physics/Collision.hpp"
#include <memory>
#include <vector>

class RigidBody;

class BroadphaseInterface {
public:
    std::shared_ptr<BroadphaseInterface> Create();
};

class CollisionConfiguration {
public:
    std::shared_ptr<CollisionConfiguration> CreateDefault();
};

class CollisionDispatcher {
public:
    std::shared_ptr<CollisionDispatcher> Create(std::shared_ptr<CollisionConfiguration> configuration);

private:
    CollisionDispatcher() = delete;
    CollisionDispatcher(std::shared_ptr<CollisionDispatcher>);
    std::shared_ptr<CollisionDispatcher> _collisionDispatcher { nullptr };
};

class PhysicsEngine {
public:
    PhysicsEngine() = default;
    void AddRigidBody(const std::shared_ptr<RigidBody>& rigidBody);
    void Simulate(float step);
    void CheckCollision();

private:
    std::vector<Collision::CollideesPair> _collideesPairs;
    std::vector<std::shared_ptr<RigidBody>> _rigidBodies;
};