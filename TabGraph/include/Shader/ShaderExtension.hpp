#pragma once

#include "Shader/Shader.hpp"
#include <string>

class ShaderExtension : public Shader {
public:
    ShaderExtension(const std::string& name);
    static std::shared_ptr<ShaderExtension> Create(const std::string &name);
    /** @returns the extension's code */
    //std::string Code() const { return _code; };
    /** @arg code : the extension's code */
    //void SetCode(const std::string& code) {
    //    _code = code;
    //};

private:
    virtual std::shared_ptr<Component> _Clone() const override {
        return tools::make_shared<ShaderExtension>(*this);
    }
    //std::string _code;
};