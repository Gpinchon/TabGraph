/*
* @Author: gpinchon
* @Date:   2021-02-20 22:19:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 11:20:25
*/
#include "Driver/OpenGL/Shader.hpp"
#include "Shader/Stage.hpp"
#include "Texture/Texture.hpp"

#include <array>
#include <string>

static inline bool IsTextureType(GLenum type)
{
    if (GL_SAMPLER_1D
        || GL_SAMPLER_2D
        || GL_SAMPLER_3D
        || GL_SAMPLER_CUBE
        || GL_SAMPLER_1D_SHADOW
        || GL_SAMPLER_2D_SHADOW
        || GL_SAMPLER_1D_ARRAY
        || GL_SAMPLER_2D_ARRAY
        || GL_SAMPLER_1D_ARRAY_SHADOW
        || GL_SAMPLER_2D_ARRAY_SHADOW
        || GL_SAMPLER_2D_MULTISAMPLE
        || GL_SAMPLER_2D_MULTISAMPLE_ARRAY
        || GL_SAMPLER_CUBE_SHADOW
        || GL_SAMPLER_BUFFER
        || GL_SAMPLER_2D_RECT
        || GL_SAMPLER_2D_RECT_SHADOW
        || GL_INT_SAMPLER_1D
        || GL_INT_SAMPLER_2D
        || GL_INT_SAMPLER_3D
        || GL_INT_SAMPLER_CUBE
        || GL_INT_SAMPLER_1D_ARRAY
        || GL_INT_SAMPLER_2D_ARRAY
        || GL_INT_SAMPLER_2D_MULTISAMPLE
        || GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
        || GL_INT_SAMPLER_BUFFER
        || GL_INT_SAMPLER_2D_RECT
        || GL_UNSIGNED_INT_SAMPLER_1D
        || GL_UNSIGNED_INT_SAMPLER_2D
        || GL_UNSIGNED_INT_SAMPLER_3D
        || GL_UNSIGNED_INT_SAMPLER_CUBE
        || GL_UNSIGNED_INT_SAMPLER_1D_ARRAY
        || GL_UNSIGNED_INT_SAMPLER_2D_ARRAY
        || GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE
        || GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY
        || GL_UNSIGNED_INT_SAMPLER_BUFFER
        || GL_UNSIGNED_INT_SAMPLER_2D_RECT
        || GL_IMAGE_1D
        || GL_IMAGE_2D
        || GL_IMAGE_3D
        || GL_IMAGE_2D_RECT
        || GL_IMAGE_CUBE
        || GL_IMAGE_BUFFER
        || GL_IMAGE_1D_ARRAY
        || GL_IMAGE_2D_ARRAY
        || GL_IMAGE_2D_MULTISAMPLE
        || GL_IMAGE_2D_MULTISAMPLE_ARRAY
        || GL_INT_IMAGE_1D
        || GL_INT_IMAGE_2D
        || GL_INT_IMAGE_3D
        || GL_INT_IMAGE_2D_RECT
        || GL_INT_IMAGE_CUBE
        || GL_INT_IMAGE_BUFFER
        || GL_INT_IMAGE_1D_ARRAY
        || GL_INT_IMAGE_2D_ARRAY
        || GL_INT_IMAGE_2D_MULTISAMPLE
        || GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY
        || GL_UNSIGNED_INT_IMAGE_1D
        || GL_UNSIGNED_INT_IMAGE_2D
        || GL_UNSIGNED_INT_IMAGE_3D
        || GL_UNSIGNED_INT_IMAGE_2D_RECT
        || GL_UNSIGNED_INT_IMAGE_CUBE
        || GL_UNSIGNED_INT_IMAGE_BUFFER
        || GL_UNSIGNED_INT_IMAGE_1D_ARRAY
        || GL_UNSIGNED_INT_IMAGE_2D_ARRAY
        || GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE
        || GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY)
        return true;
    else
        return false;
}

std::array<GLenum, 6> GLStageLUT = {
    GL_GEOMETRY_SHADER, //Geometry
    GL_VERTEX_SHADER, //Vertex
    GL_FRAGMENT_SHADER, //Fragment
    GL_COMPUTE_SHADER, //Compute
    GL_TESS_EVALUATION_SHADER, //TessellationEvaluation
    GL_TESS_CONTROL_SHADER //TessellationControl
};

namespace Shader {
static std::map<std::string, std::shared_ptr<Texture>> s_globalTextures;
static std::map<std::string, std::string> s_globalDefines;
static std::map<std::string, std::vector<float>> s_globalFloats;
static std::map<std::string, std::vector<int32_t>> s_globalInts;
static std::map<std::string, std::vector<uint32_t>> s_globalUints;
Program::Impl::~Impl()
{
    glDeleteShader(GetHandle());
}
void Program::Impl::Attach(const Stage& stage)
{
    _stages.at((int)stage.GetType()) = stage;
}
Stage& Program::Impl::GetStage(Stage::Type type)
{
    return _stages.at((int)type);
}
void Program::Impl::Compile()
{
    if (GetCompiled())
        return;
    _SetHandle(glCreateProgram());
    std::array<GLuint, (int)Stage::Type::MaxValue> handles { 0 };
    for (const auto& stage : _stages) {
        if (stage.GetCode().code.empty() && stage.GetCode().technique.empty())
            continue;
        std::string fullCode;
        for (const auto& define : _defines)
            fullCode += "#define " + define.first + ' ' + define.second + '\n';
        for (const auto& define : stage.GetDefines())
            fullCode += "#define " + define.first + ' ' + define.second + '\n';
        if (!stage.GetCode().code.empty())
            fullCode += stage.GetCode().code + '\n';
        fullCode += "void main(void) {\n";
        if (!stage.GetCode().technique.empty())
            fullCode += stage.GetCode().technique + '\n';
        fullCode += "}\n";
        auto codeBuff { fullCode.c_str() };
        auto handle = handles.at((int)stage.GetType()) = glCreateShader(GLStageLUT.at((int)stage.GetType()));
        glShaderSource(handle, 1, &codeBuff, nullptr);
        glCompileShader(handle);
        glAttachShader(GetHandle(), handle);
    }
    glLinkProgram(GetHandle());
    for (const auto& handle : handles) {
        if (handle == 0)
            continue;
        glDetachShader(GetHandle(), handle);
        glDeleteShader(handle);
    }
    GLint numUniforms = 0;
    glGetProgramInterfaceiv(GetHandle(), GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    const GLenum properties[4] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
    uint16_t textureIndex { 0 };
    for (int unif = 0; unif < numUniforms; ++unif) {
        GLint values[4];
        glGetProgramResourceiv(GetHandle(), GL_UNIFORM, unif, 4, properties, 4, NULL, values);
        // Skip any uniforms that are in a block.
        if (values[0] != -1)
            continue;
        std::string name(values[2], '\0');
        glGetProgramResourceName(GetHandle(), GL_UNIFORM, unif, name.size(), NULL, name.data());
        _uniformLocs[name] = properties[3];
        if (IsTextureType(properties[1])) {
            _textureIndice[properties[3]] = textureIndex;
            ++textureIndex;
        }
    }
    _SetCompiled(true);
}
void Program::Impl::SetUniform(const std::string& name, const std::shared_ptr<Texture> value)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end()) {
        auto index { _textureIndice.find(loc->second) };
        if (index != _textureIndice.end()) {
            glActiveTexture(index->second);
            if (value != nullptr)
                glBindTexture((GLenum)value->GetType(), value->GetHandle());
            else {
                //TODO properly reset sampler
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glUniform1i(loc->second, index->second - GL_TEXTURE0);
        }
    }
}
void Program::Impl::SetUniform(const std::string& name, const float value)
{
    SetUniform(name, &value, 1);
}
void Program::Impl::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1fv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const int32_t value)
{
    SetUniform(name, &value, 1);
}
void Program::Impl::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1iv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const uint32_t value)
{
    SetUniform(name, &value, 1);
}
void Program::Impl::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1uiv(loc->second + index, count, value);
}
void Program::Impl::SetDefine(const std::string& name, const std::string& value)
{
    _defines[name] = value;
    glDeleteProgram(GetHandle());
    _SetHandle(0);
    _SetCompiled(false);
}
void Program::Impl::RemoveDefine(const std::string& name)
{
    _defines.erase(name);
}
void Program::Impl::Use()
{
    if (!GetCompiled())
        Compile();
    glUseProgram(GetHandle());
}
void Program::Impl::UseNone()
{
    glUseProgram(0);
}
namespace Global {
    namespace Impl {
        void SetUniform(const std::string& name, const std::shared_ptr<Texture> value)
        {
            s_globalTextures[name] = value;
        }
        void SetUniform(const std::string& name, const float value)
        {
            SetUniform(name, &value, 1);
        }
        void SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
        {
            auto& values { s_globalFloats[name] };
            if (values.size() <= index + count)
                values.resize(index + count, 0);
            values.insert(values.begin() + index, value, value + count);
        }
        void SetUniform(const std::string& name, const int32_t value)
        {
            SetUniform(name, &value, 1);
        }
        void SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
        {
            auto& values { s_globalInts[name] };
            if (values.size() <= index + count)
                values.resize(index + count, 0);
            values.insert(values.begin() + index, value, value + count);
        }
        void SetUniform(const std::string& name, const uint32_t value)
        {
            SetUniform(name, &value, 1);
        }
        void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
        {
            auto& values { s_globalUints[name] };
            if (values.size() <= index + count)
                values.resize(index + count, 0);
            values.insert(values.begin() + index, value, value + count);
        }
        void SetDefine(const std::string& name, const std::string& value)
        {
            s_globalDefines[name] = value;
        }
        void RemoveDefine(const std::string& name)
        {
            s_globalDefines.erase(name);
        }
    };
};
};