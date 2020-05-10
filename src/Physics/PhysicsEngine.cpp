#include "Physics/PhysicsEngine.hpp"
#include "Physics/RigidBody.hpp"
#include "Node.hpp"
#include <algorithm>

void PhysicsEngine::AddRigidBody(const std::shared_ptr<RigidBody> &rigidBody)
{
	_rigidBodies.push_back(rigidBody);
}

void PhysicsEngine::Simulate(float step)
{
	for (auto &rigidBody : _rigidBodies) {
		auto node(rigidBody->GetNode());
		rigidBody->IntegrateVelocities(step);
		if (node == nullptr) continue;
		node->SetPosition(node->Position() + rigidBody->LinearVelocity() * step);
	    node->SetRotation(normalize(node->Rotation() + step * 0.5f * rigidBody->AngularSpin() * node->Rotation()));
	    node->UpdateTransformMatrix();
	    rigidBody->GetCollider()->Transform(node->WorldPosition(), node->WorldRotation(), node->WorldScale());
	}
}

#include <iostream>

void PhysicsEngine::CheckCollision()
{
	_collideesPairs.clear();
	for (auto &rigidBodya : _rigidBodies) {
		for (auto &rigidBodyb : _rigidBodies) {
			if (rigidBodya == rigidBodyb)
				continue;
			auto intersectionResult(rigidBodya->GetCollider()->Intersect(rigidBodyb->GetCollider()));
			if (intersectionResult.GetIntersects()) {
				CollideesPair pair(rigidBodya, rigidBodyb);
				if(std::find(_collideesPairs.begin(), _collideesPairs.end(), pair) == _collideesPairs.end())
					_collideesPairs.push_back(pair);
				std::cout << intersectionResult.GetIntersects() << " " << intersectionResult.GetDistance() << std::endl;
			}
		}
	}
}