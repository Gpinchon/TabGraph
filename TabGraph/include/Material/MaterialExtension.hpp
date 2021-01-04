#pragma once
#include "Component.hpp"
#include "Shader/ShaderExtension.hpp"

class Texture2D;

class MaterialExtension : public Component
{
public:
	virtual std::shared_ptr<ShaderExtension> GetShaderExtension() const final;

protected:
	MaterialExtension(const std::string& name);
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<MaterialExtension>(*this);
    }
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
};