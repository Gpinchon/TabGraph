#pragma once

#include "GameEntity.hpp"

class Player : public GameEntity {
public:
    static std::shared_ptr<Player> Create(const glm::vec3& color);
    void Move(const glm::vec2& direction, float delta);
    float Speed() const;
    void SetSpeed(float speed);

private:
    virtual void _FixedUpdateCPU(float delta) override;
    Player(const std::string& name, const glm::vec3& color);
    float _speed { 5.f };
};