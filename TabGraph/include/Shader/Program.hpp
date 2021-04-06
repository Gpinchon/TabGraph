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

#include <glm/fwd.hpp>
#include <string>

class Texture;

namespace Shader {
class Program : public Component {
    class Handle;
    class Impl;
    friend Impl;
    friend Handle;
    //std::unique_ptr<Impl> _impl;
    PRIVATEPROPERTY(std::shared_ptr<Impl>, Impl, nullptr);

public:
    Program();
    //Program(const Program &other);
    Program(const std::string& name);
    ~Program();
    const Handle &GetHandle() const;
    Stage& GetStage(Stage::Type);
    Program& Compile();
    Program& Attach(const Stage &stage);
    Program& Use();
    Program& SetTexture(const std::string& name, const std::shared_ptr<Texture> value);
    Program& SetUniform(const std::string& name, const float value);
    Program& SetUniform(const std::string& name, const float* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const int32_t value);
    Program& SetUniform(const std::string& name, const int32_t* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const uint32_t value);
    Program& SetUniform(const std::string& name, const uint32_t* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::vec2& value);
    Program& SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::vec3& value);
    Program& SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::vec4& value);
    Program& SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::mat4& value);
    Program& SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetUniform(const std::string& name, const glm::mat3& value);
    Program& SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count = 1, const uint16_t index = 0);
    Program& SetDefine(const std::string& name, const std::string& value = "");
    Program& RemoveDefine(const std::string& name);
    void Done();
    bool GetCompiled() const;
    static void UseNone();

private :
    
    virtual std::shared_ptr<Component> _Clone() override
    {
        auto program(Component::Create<Program>(*this));
        return program;
    }
};
};
