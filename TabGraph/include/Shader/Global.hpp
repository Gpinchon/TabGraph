#pragma once

#include <string>
#include <memory>

class Texture;

namespace Shader {
namespace Global {
    void SetUniform(const std::string& name, const std::shared_ptr<Texture> value);
    void SetUniform(const std::string& name, const float value);
    void SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index = 0);
    void SetUniform(const std::string& name, const int32_t value);
    void SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index = 0);
    void SetUniform(const std::string& name, const uint32_t value);
    void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index = 0);
    void SetDefine(const std::string& name, const std::string& value);
    void RemoveDefine(const std::string& name);
};
};