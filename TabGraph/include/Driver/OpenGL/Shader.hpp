/*
* @Author: gpinchon
* @Date:   2021-02-20 22:20:14
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-20 22:52:20
*/
#pragma once

#include "Shader/Program.hpp"
#include "Shader/Stage.hpp"

#include <array>

class Texture;

namespace Shader {
class Program::Impl {
    using Handle = uint32_t;
    READONLYPROPERTY(Impl::Handle, Handle, 0);
    READONLYPROPERTY(bool, Compiled, false);

public:
    ~Impl();
    void Attach(const Stage& stage);
    Stage& GetStage(Stage::Type);
    void Compile();
    void SetUniform(const std::string& name, const std::shared_ptr<Texture> value);
    void SetUniform(const std::string& name, const float value);
    void SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index = 0);
    void SetUniform(const std::string& name, const int32_t value);
    void SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index = 0);
    void SetUniform(const std::string& name, const uint32_t value);
    void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index = 0);
    void SetDefine(const std::string& name, const std::string& value);
    void RemoveDefine(const std::string& name);
    void Use();
    static void UseNone();

private:
    std::array<Stage, (int)Stage::Type::MaxValue> _stages;
    std::map<std::string, std::string> _defines;
    std::map<std::string, int32_t> _uniformLocs;
    std::map<int32_t, int32_t> _textureIndice;
};
namespace Global {
    namespace Impl {
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
};
