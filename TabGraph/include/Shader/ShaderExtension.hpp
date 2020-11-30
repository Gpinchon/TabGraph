#pragma once

#include "Shader/Shader.hpp"
#include <string>

//TODO reimplement update functions and avoid compilation attempt

class ShaderExtension : public Shader {
public:
    ShaderExtension(const std::string& name);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<ShaderExtension>(*this);
    }
};