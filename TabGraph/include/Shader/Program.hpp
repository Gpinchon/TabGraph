/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-19 20:24:10
*/
#pragma once

#include "Component.hpp" // for Component
#include "Property.hpp"
#include "Shader/Stage.hpp"

#include <string>

class Texture;

namespace Shader {
class Program : public Component {
public:
    class Impl;
    PRIVATEPROPERTY(Impl*, Impl, nullptr);
    READONLYPROPERTY(bool, Compiled, false);

public:
    Program();
    Program(const std::string& name);
    ~Program();
    Stage& GetStage(Stage::Type);
    Program& Compile();
    Program& Attach(const Stage &stage);
    Program& Use();
    Program& SetUniform(const std::string& name, const std::shared_ptr<Texture> value);
    Program& SetUniform(const std::string& name, const float value);
    Program& SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const int32_t value);
    Program& SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const uint32_t value);
    Program& SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index = 0);
    Program& SetDefine(const std::string& name, const std::string& value = "");
    Program& RemoveDefine(const std::string& name);
    void Done();
    static void UseNone();
private :
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto program(Component::Create<Program>(*this));
        return program;
    }
};
};
