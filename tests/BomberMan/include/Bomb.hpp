#pragma once

#include "GameEntity.hpp"

#include <chrono>

class Bomb : public GameEntity {
public:
    static std::shared_ptr<Bomb> Create(const glm::ivec2& position);
    virtual void Die() override;
    void ResetTimer();
    std::chrono::time_point<std::chrono::high_resolution_clock> SpawnTime() const;
    std::chrono::duration<double> Timer() const;
    void SetTimer(std::chrono::duration<double> timer);
    int Range() const;
    void SetRange(int range);
    ~Bomb() { std::cout << "BOOM !" << std::endl; };

private:
    Bomb();

    virtual void _FixedUpdateCPU(float delta) override;
    int _range { 5 };
    std::chrono::duration<double> _timer { 3 };
    std::chrono::time_point<std::chrono::high_resolution_clock> _spawnTime;
};