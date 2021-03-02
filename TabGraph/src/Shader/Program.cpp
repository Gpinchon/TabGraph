/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-20 22:55:26
*/

#include "Shader/Program.hpp"
#include "Driver/OpenGL/Shader.hpp"
#include "Shader/Stage.hpp"

namespace Shader {
    static auto shaderProgramNbr = 0u;
Program::Program() : Component("Shader_" + std::to_string(++shaderProgramNbr))
{
    _SetImpl(std::make_shared<Program::Impl>());
}

Program::Program(const std::string& name) : Program()
{
    SetName(name);
}

Program::~Program()
{
}

const Program::Handle &Program::GetHandle() const
{
    return _GetImpl()->GetHandle();
}

Stage& Program::GetStage(Stage::Type type)
{
    return _GetImpl()->GetStage(type);
}

Program& Program::Compile()
{
    _GetImpl()->Compile();
    return *this;
}

Program& Program::Attach(const Stage& stage)
{
    _GetImpl()->Attach(stage);
    return *this;
}

Program& Program::Use()
{
    _GetImpl()->Use();
    return *this;
}

Program& Program::SetTexture(const std::string& name, const std::shared_ptr<Texture> value)
{
    _GetImpl()->SetTexture(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const float value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const int32_t value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const uint32_t value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec2& value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec3& value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec4& value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat4& value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat3& value)
{
    _GetImpl()->SetUniform(name, value);
    return *this;
}

Program& Program::SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index)
{
    _GetImpl()->SetUniform(name, value, count, index);
    return *this;
}

Program& Program::SetDefine(const std::string& name, const std::string& value)
{
    _GetImpl()->SetDefine(name, value);
    return *this;
}

Program& Program::RemoveDefine(const std::string& name)
{
    _GetImpl()->RemoveDefine(name);
    return *this;
}

void Program::Done()
{
    UseNone();
}

bool Program::GetCompiled() const
{
    return _GetImpl()->GetCompiled();
}

void Program::UseNone()
{
    Program::Impl::UseNone();
}
};
