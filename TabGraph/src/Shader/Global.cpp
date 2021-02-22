/*
* @Author: gpinchon
* @Date:   2021-02-22 11:10:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 11:22:01
*/
#include "Shader/Global.hpp"
#include "Driver/OpenGL/Shader.hpp"

namespace Shader {
namespace Global {
    void Global::SetUniform(const std::string& name, const std::shared_ptr<Texture> value)
    {
        Impl::SetUniform(name, value);
    }

    void Global::SetUniform(const std::string& name, const float value)
    {
        Impl::SetUniform(name, value);
    }

    void Global::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
    {
        Impl::SetUniform(name, value, count, index);
    }

    void Global::SetUniform(const std::string& name, const int32_t value)
    {
        Impl::SetUniform(name, value);
    }

    void Global::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
    {
        Impl::SetUniform(name, value, count, index);
    }

    void Global::SetUniform(const std::string& name, const uint32_t value)
    {
        Impl::SetUniform(name, value);
    }

    void Global::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
    {
        Impl::SetUniform(name, value, count, index);
    }

    void Global::SetDefine(const std::string& name, const std::string& value)
    {
        Impl::SetDefine(name, value);
    }
    void Global::RemoveDefine(const std::string& name)
    {
        Impl::RemoveDefine(name);
    }
}
};