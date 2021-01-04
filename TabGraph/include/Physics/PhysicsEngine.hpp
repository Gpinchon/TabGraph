/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 23:37:50
*/
#pragma once

#include "Component.hpp"
#include "Physics/Collision.hpp"

#include <memory>
#include <vector>

class RigidBody;

class BroadphaseInterface {
public:
    static std::shared_ptr<BroadphaseInterface> Create();
};

class CollisionConfiguration {
public:
    static std::shared_ptr<CollisionConfiguration> CreateDefault();
};

class CollisionDispatcher : public Component {
public:
    static std::shared_ptr<CollisionDispatcher> Create(std::shared_ptr<CollisionConfiguration> configuration);

private:
    CollisionDispatcher() = delete;
    CollisionDispatcher(std::shared_ptr<CollisionDispatcher>);
    std::shared_ptr<CollisionDispatcher> _collisionDispatcher { nullptr };
};

class PhysicsEngine : public Component {
public:
    static std::shared_ptr<PhysicsEngine> Create();
    void AddRigidBody(const std::shared_ptr<RigidBody>& rigidBody);
    void Simulate(float step);
    void CheckCollision();

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<PhysicsEngine>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override;
    std::vector<Collision::CollideesPair> _collideesPairs;
    std::vector<std::shared_ptr<RigidBody>> _rigidBodies;
};