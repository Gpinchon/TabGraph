#pragma once

#include "Node.hpp"

class RigidBody;

class GameEntity : public Node {
public:
    std::string Type() const;
    glm::vec2 Size() const;
    float Height() const;
    void SetPosition(const glm::vec2& position);
    glm::vec2 Position() const;
    void LookAt(const glm::vec2& position);

protected:
    GameEntity(const std::string& name, const std::string& entityType);
    glm::vec2 _size { 1.f };
    float _height { 1.f };

private:
    const std::string _type;
};