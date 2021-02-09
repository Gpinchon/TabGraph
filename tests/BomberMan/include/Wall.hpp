#pragma once

#include "GameEntity.hpp"

class Wall : public GameEntity {
public:
    Wall();
    static std::shared_ptr<Wall> Create();
private:
    virtual std::shared_ptr<Component> _Clone() override {
        return std::shared_ptr<Wall>(new Wall(*this));
    }
};