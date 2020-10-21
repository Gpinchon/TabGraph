#pragma once

#include "Shader/Shader.hpp"
#include <string>

class ShaderExtension : public Shader {
public:
    static std::shared_ptr<ShaderExtension> Create(const std::string &name);
    /** @returns the extension's code */
    //std::string Code() const { return _code; };
    /** @arg code : the extension's code */
    //void SetCode(const std::string& code) {
    //    _code = code;
    //};

private:
    ShaderExtension(const std::string& name);
    //std::string _code;
};