#pragma once

#include <string>
#include <memory>
#include <glm/fwd.hpp>

class Texture;

namespace Shader {
namespace Global {
    void SetTexture(const std::string& name, const std::shared_ptr<Texture> value);
    void SetUniform(const std::string& name, const float value);
    void SetUniform(const std::string& name, const float* value, const uint16_t count = 1, const uint16_t index = 0);
    void SetUniform(const std::string& name, const int32_t value);
    void SetUniform(const std::string& name, const int32_t* value, const uint16_t count = 1, const uint16_t index = 0);
    void SetUniform(const std::string& name, const uint32_t value);
    void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count = 1, const uint16_t index = 0);
    void SetUniform(const std::string& name, const glm::vec2& value);
    void SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index);
    void SetUniform(const std::string& name, const glm::vec4& value);
    void SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index);
    void SetUniform(const std::string& name, const glm::mat4& value);
    void SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index);
    void SetUniform(const std::string& name, const glm::mat3& value);
    void SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index);
    void SetDefine(const std::string& name, const std::string& value);
    void RemoveDefine(const std::string& name);
};
};