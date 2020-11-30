#pragma once

#include "Component.hpp"

class AssetsContainer : public Component {
private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<AssetsContainer>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
};
