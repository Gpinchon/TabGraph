#pragma once

#include "GameEntity.hpp"

class SDL_KeyboardEvent;

class Player : public GameEntity {
public:
    Player(const std::string& name, const glm::vec3& color);
    static std::shared_ptr<Player> Create(const glm::vec3& color);
    void Move(const glm::vec2& direction, float delta);
    float Speed() const;
    void SetSpeed(float speed);
    void DropBomb(const SDL_KeyboardEvent& event) const;
    virtual void Die() override;

private:
    virtual void _FixedUpdateCPU(float delta) override;
    float _speed { 4.f };
};