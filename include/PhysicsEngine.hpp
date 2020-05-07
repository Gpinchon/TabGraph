#pragma once

#include <vector>
#include <memory>

class RigidBody;

class CollideesPair
{
public:
	CollideesPair(const std::shared_ptr<RigidBody> &first, const std::shared_ptr<RigidBody> &second) : _first(first), _second(second) {};
	bool operator == (const CollideesPair &other) const;
	std::shared_ptr<RigidBody> First() const { return _first; };
	std::shared_ptr<RigidBody> Second() const { return _second; };
private:
	const std::shared_ptr<RigidBody> &_first;
	const std::shared_ptr<RigidBody> &_second;
};

inline bool CollideesPair::operator == (const CollideesPair &other) const
{
	if (First() == other.First() || First() == other.Second()) {
		return Second() == other.First() || Second() == other.Second();
	}
	return false;
}

class PhysicsEngine
{
public:
	PhysicsEngine() = default;
	void AddRigidBody(const std::shared_ptr<RigidBody> &rigidBody);
	void Simulate(float step);
	void CheckCollision();
private:
	std::vector<CollideesPair> _collideesPairs;
	std::vector<std::shared_ptr<RigidBody>> _rigidBodies;
	
};