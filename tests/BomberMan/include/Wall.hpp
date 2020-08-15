#pragma once

#include "GameEntity.hpp"

class Wall : public GameEntity {
public:
    static std::shared_ptr<Wall> Create();

private:
    Wall();
};