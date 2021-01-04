#pragma once

#include "Node.hpp"

class RigidBody;

class GameEntity : public Node {
public:
    GameEntity(const GameEntity& entity);
    GameEntity(const std::string& name, const std::string& entityType);
    GameEntity(const std::string& entityType);
    std::string Type() const;
    glm::vec2 Size() const;
    float Height() const;
    void SetPosition(const glm::vec2& position);
    glm::vec2 Position() const;
    void LookAt(const glm::vec2& position);
    virtual void Die();
    void AddAnimation(const std::shared_ptr<Animation> &animation);
    void PlayAnimation(const std::string& name, bool repeat = false);

protected:
    glm::vec2 _size { 1.f };
    float _height { 1.f };

private:
    const std::string _type;
    std::unordered_map<std::string, std::weak_ptr<Animation>> _animations;
    std::weak_ptr<Animation> _currentAnimation;
};