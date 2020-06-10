#include "Physics/PhysicsEngine.hpp"
#include "Node.hpp"
#include "Physics/RigidBody.hpp"
#include "Transform.hpp"
#include <algorithm>

void PhysicsEngine::AddRigidBody(const std::shared_ptr<RigidBody>& rigidBody)
{
    _rigidBodies.push_back(rigidBody);
}

void PhysicsEngine::Simulate(float step)
{
    for (auto& rigidBody : _rigidBodies) {
        auto node(rigidBody->GetNode());
        rigidBody->IntegrateVelocities(step);
        if (node == nullptr || node->GetComponent<Transform>() == nullptr)
            continue;
        auto transform(node->GetComponent<Transform>());
        transform->SetPosition(transform->Position() + rigidBody->LinearVelocity() * step);
        transform->SetRotation(normalize(transform->Rotation() + step * 0.5f * rigidBody->AngularSpin() * transform->Rotation()));
        //transform->UpdateTransformMatrix();
        rigidBody->GetCollider()->Transform(transform->WorldPosition(), transform->WorldRotation(), transform->WorldScale());
    }
}

#include <iostream>

void PhysicsEngine::CheckCollision()
{
    _collideesPairs.clear();
    for (auto& rigidBodya : _rigidBodies) {
        for (auto& rigidBodyb : _rigidBodies) {
            if (rigidBodya == rigidBodyb)
                continue;
            auto intersectionResult(rigidBodya->GetCollider()->Intersect(rigidBodyb->GetCollider()));
            if (intersectionResult.GetIntersects()) {
                CollideesPair pair(rigidBodya, rigidBodyb);
                if (std::find(_collideesPairs.begin(), _collideesPairs.end(), pair) == _collideesPairs.end())
                    _collideesPairs.push_back(pair);
                std::cout << intersectionResult.GetIntersects() << " " << intersectionResult.GetDistance() << std::endl;
            }
        }
    }
}