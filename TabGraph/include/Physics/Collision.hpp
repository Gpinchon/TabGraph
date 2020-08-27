#pragma once

#include <glm/glm.hpp>
#include <memory>

class RigidBody;

class Collision {
public:
    class CollideesPair {
    public:
        CollideesPair() = default;
        CollideesPair(const std::shared_ptr<RigidBody>& first, const std::shared_ptr<RigidBody>& second)
            : _first(first)
            , _second(second)
        {
        }
        bool operator==(const CollideesPair& other) const
        {
            if (First() == other.First() || First() == other.Second()) {
                return Second() == other.First() || Second() == other.Second();
            }
            return false;
        }
        std::shared_ptr<RigidBody> First() const { return _first; }
        std::shared_ptr<RigidBody> Second() const { return _second; }

    private:
        std::shared_ptr<RigidBody> _first { nullptr };
        std::shared_ptr<RigidBody> _second { nullptr };
    };
    Collision() = default;
    Collision(const CollideesPair& collidees, const glm::vec3& position, const glm::vec3& normal, const float& penetrationDepth)
        : _collidees(collidees)
        , _position(position)
        , _normal(normal)
        , _penetrationDepth(penetrationDepth)
    {
    }
    auto Collidees() const { return _collidees; }
    auto Position() const { return _position; }
    auto Normal() const { return _normal; }
    auto PenetrationDepth() const { return _penetrationDepth; }
    bool operator==(const Collision& other)
    {
        return Collidees() == other.Collidees() && glm::distance(Position(), other.Position()) <= 0.0001;
    }

private:
    CollideesPair _collidees;
    glm::vec3 _position { std::numeric_limits<float>::infinity() };
    glm::vec3 _normal { 0.f };
    float _penetrationDepth { std::numeric_limits<float>::infinity() };
};