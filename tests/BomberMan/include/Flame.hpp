#pragma once

#include "GameEntity.hpp"

#include <chrono>

class Flame : public GameEntity {
public:
    static std::shared_ptr<Flame> Create(const glm::ivec2& position);
    std::chrono::time_point<std::chrono::high_resolution_clock> SpawnTime() const;
    std::chrono::duration<double> Timer() const;
    void SetTimer(std::chrono::duration<double> timer);

private:
    Flame();
    virtual void _FixedUpdateCPU(float delta) override;
    std::chrono::duration<double> _timer { 0.5 }; //defaults to seconds
    const std::chrono::time_point<std::chrono::high_resolution_clock> _spawnTime;
};