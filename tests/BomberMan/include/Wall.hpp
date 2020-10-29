#pragma once

#include "GameEntity.hpp"

class Wall : public GameEntity {
public:
    Wall();
    static std::shared_ptr<Wall> Create();
};