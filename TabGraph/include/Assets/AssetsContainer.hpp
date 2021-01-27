#pragma once

#include "Component.hpp"

class AssetsContainer : public Component {
private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<AssetsContainer>(*this);
    }
};
