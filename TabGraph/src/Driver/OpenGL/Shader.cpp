/*
* @Author: gpinchon
* @Date:   2021-02-20 22:19:57
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-23 19:22:14
*/
#include "Driver/OpenGL/Shader.hpp"
#include "Shader/Stage.hpp"
#include "Texture/Texture.hpp"

#include <array>
#include <string>
#include <glm/glm.hpp>

static inline bool IsTextureType(GLenum type)
{
    switch (type) {
    case GL_SAMPLER_1D:
    case GL_SAMPLER_2D:
    case GL_SAMPLER_3D:
    case GL_SAMPLER_CUBE:
    case GL_SAMPLER_1D_SHADOW:
    case GL_SAMPLER_2D_SHADOW:
    case GL_SAMPLER_1D_ARRAY:
    case GL_SAMPLER_2D_ARRAY:
    case GL_SAMPLER_1D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_ARRAY_SHADOW:
    case GL_SAMPLER_2D_MULTISAMPLE:
    case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_SAMPLER_CUBE_SHADOW:
    case GL_SAMPLER_BUFFER:
    case GL_SAMPLER_2D_RECT:
    case GL_SAMPLER_2D_RECT_SHADOW:
    case GL_INT_SAMPLER_1D:
    case GL_INT_SAMPLER_2D:
    case GL_INT_SAMPLER_3D:
    case GL_INT_SAMPLER_CUBE:
    case GL_INT_SAMPLER_1D_ARRAY:
    case GL_INT_SAMPLER_2D_ARRAY:
    case GL_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_INT_SAMPLER_BUFFER:
    case GL_INT_SAMPLER_2D_RECT:
    case GL_UNSIGNED_INT_SAMPLER_1D:
    case GL_UNSIGNED_INT_SAMPLER_2D:
    case GL_UNSIGNED_INT_SAMPLER_3D:
    case GL_UNSIGNED_INT_SAMPLER_CUBE:
    case GL_UNSIGNED_INT_SAMPLER_1D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_SAMPLER_BUFFER:
    case GL_UNSIGNED_INT_SAMPLER_2D_RECT:
    case GL_IMAGE_1D:
    case GL_IMAGE_2D:
    case GL_IMAGE_3D:
    case GL_IMAGE_2D_RECT:
    case GL_IMAGE_CUBE:
    case GL_IMAGE_BUFFER:
    case GL_IMAGE_1D_ARRAY:
    case GL_IMAGE_2D_ARRAY:
    case GL_IMAGE_2D_MULTISAMPLE:
    case GL_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_INT_IMAGE_1D:
    case GL_INT_IMAGE_2D:
    case GL_INT_IMAGE_3D:
    case GL_INT_IMAGE_2D_RECT:
    case GL_INT_IMAGE_CUBE:
    case GL_INT_IMAGE_BUFFER:
    case GL_INT_IMAGE_1D_ARRAY:
    case GL_INT_IMAGE_2D_ARRAY:
    case GL_INT_IMAGE_2D_MULTISAMPLE:
    case GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_1D:
    case GL_UNSIGNED_INT_IMAGE_2D:
    case GL_UNSIGNED_INT_IMAGE_3D:
    case GL_UNSIGNED_INT_IMAGE_2D_RECT:
    case GL_UNSIGNED_INT_IMAGE_CUBE:
    case GL_UNSIGNED_INT_IMAGE_BUFFER:
    case GL_UNSIGNED_INT_IMAGE_1D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_ARRAY:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
    case GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        return true;
    default:
        return false;
    }
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
static std::map<std::string, std::string> s_globalDefines;
static std::map<std::string, std::vector<float>> s_globalFloats;
static std::map<std::string, std::vector<glm::vec2>> s_globalVec2;
static std::map<std::string, std::vector<glm::vec3>> s_globalVec3;
static std::map<std::string, std::vector<glm::vec4>> s_globalVec4;
static std::map<std::string, std::vector<glm::mat4>> s_globalMat4;
static std::map<std::string, std::vector<glm::mat3>> s_globalMat3;
static std::map<std::string, std::vector<int32_t>> s_globalInts;
static std::map<std::string, std::vector<uint32_t>> s_globalUints;
static std::map<std::string, std::shared_ptr<Texture>> s_globalTextures;
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
        std::string fullCode = "#version " + GetGLSLVersion() + '\n';
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
        GLint result;
        GLint loglength;
        result = GL_FALSE;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &result);
        if (result != GL_TRUE) {
            glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &loglength);
            if (loglength > 1) {
                std::vector<char> log(loglength, 0);
                glGetShaderInfoLog(handle, loglength, nullptr, log.data());
                std::string logString(log.begin(), log.end());
                throw std::runtime_error(logString);
            }
            else {
                throw std::runtime_error("Unknown Error");
            }
        }
        glAttachShader(GetHandle(), handle);
    }
    glLinkProgram(GetHandle());
    GLint result;
    GLint loglength;

    result = GL_FALSE;
    glGetProgramiv(GetHandle(), GL_LINK_STATUS, &result);
    if (result != GL_TRUE) {
        glGetProgramiv(GetHandle(), GL_INFO_LOG_LENGTH, &loglength);
        if (loglength > 1) {
            std::vector<char> log(loglength, 0);
            glGetProgramInfoLog(GetHandle(), loglength, nullptr, log.data());
            std::string logString(log.begin(), log.end());
            throw std::runtime_error(logString);
        }
        else {
            throw std::runtime_error("Unknown Error");
        }
    }
    for (const auto& handle : handles) {
        if (handle == 0)
            continue;
        glDetachShader(GetHandle(), handle);
        glDeleteShader(handle);
    }
    GLint numUniforms = 0;
    glGetProgramiv(GetHandle(), GL_ACTIVE_UNIFORMS, &numUniforms);
    //glGetProgramInterfaceiv(GetHandle(), GL_UNIFORM, GL_ACTIVE_RESOURCES, &numUniforms);
    //const GLenum properties[4] = { GL_BLOCK_INDEX, GL_TYPE, GL_NAME_LENGTH, GL_LOCATION };
    uint16_t textureIndex { 0 };
    char name[4096];
    while (--numUniforms >= 0) {
        //GLint values[4];
        GLenum type;
        GLint size;
        glGetActiveUniform(GetHandle(), static_cast<GLuint>(numUniforms), 4096, nullptr, &size, &type, name);
        auto loc{ glGetUniformLocation(GetHandle(), name) };
        _uniformLocs[name] = loc;
        if (IsTextureType(type))
            _textureIndice[loc] = ++textureIndex;
        //glGetProgramResourceiv(GetHandle(), GL_UNIFORM, unif, 4, properties, 4, NULL, values);
        // Skip any uniforms that are in a block.
        //if (values[0] != -1)
        //    continue;
        //std::string name(values[2] - 1, '\0');
        //glGetProgramResourceName(GetHandle(), GL_UNIFORM, unif, name.size() + 1, NULL, name.data());
        //_uniformLocs[name] = values[3];
        //if (IsTextureType(values[1])) {
            //_textureIndice[values[3]] = textureIndex;
            //++textureIndex;
        //}
    }
    _SetCompiled(true);
}
void Program::Impl::SetTexture(const std::string& name, const std::shared_ptr<Texture> value)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end()) {
        auto index { _textureIndice.find(loc->second) };
        if (index != _textureIndice.end()) {
            glActiveTexture(GL_TEXTURE0 + index->second);
            if (value != nullptr) {
                value->Load();
                glBindTexture((GLenum)value->GetType(), value->GetHandle());
            }
            else {
                //TODO properly reset sampler
                glBindTexture(GL_TEXTURE_2D, 0);
            }
            glUniform1i(loc->second, index->second);
        }
    }
}
void Program::Impl::SetUniform(const std::string& name, const float value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1fv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const int32_t value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1iv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const uint32_t value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
{
    auto loc { _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform1uiv(loc->second + index, count, value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec2& value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
{
    auto loc{ _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform2fv(loc->second + index, count, (float*)value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec3& value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
{
    auto loc{ _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform3fv(loc->second + index, count, (float*)value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec4& value)
{
    SetUniform(name, &value, 1, 0);
}
void Program::Impl::SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
{
    auto loc{ _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniform4fv(loc->second + index, count, (float*)value);
}
void Program::Impl::SetUniform(const std::string& name, const glm::mat4& value)
{
    SetUniform(name, &value, 1, 0);
}

void Program::Impl::SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
{
    auto loc{ _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniformMatrix4fv(loc->second + index, count, false, (float*)value);
}

void Program::Impl::SetUniform(const std::string& name, const glm::mat3& value)
{
    SetUniform(name, &value, 1, 0);
}

void Program::Impl::SetUniform(const std::string& name, const glm::mat3* value, const uint16_t count, const uint16_t index)
{
    auto loc{ _uniformLocs.find(name) };
    if (loc != _uniformLocs.end())
        glUniformMatrix3fv(loc->second + index, count, false, (float*)value);
}

void Program::Impl::SetDefine(const std::string& name, const std::string& value)
{
    auto& define { _defines.find(name) };
    if (define == _defines.end() || define->second != value) {
        _defines[name] = value;
        glDeleteProgram(GetHandle());
        _SetHandle(0);
        _SetCompiled(false);
    }
}
void Program::Impl::RemoveDefine(const std::string& name)
{
    if (_defines.erase(name) > 0) {
        glDeleteProgram(GetHandle());
        _SetHandle(0);
        _SetCompiled(false);
    }
}
void Program::Impl::Use()
{
    for (const auto& globalDefine : s_globalDefines)
        SetDefine(globalDefine.first, globalDefine.second);
    if (!GetCompiled())
        Compile();
    glUseProgram(GetHandle());
    for (const auto& globalVec2 : s_globalVec2)
        SetUniform(globalVec2.first, globalVec2.second.data(), globalVec2.second.size(), 0);
    for (const auto& globalVec3 : s_globalVec3)
        SetUniform(globalVec3.first, globalVec3.second.data(), globalVec3.second.size(), 0);
    for (const auto& globalVec4 : s_globalVec4)
        SetUniform(globalVec4.first, globalVec4.second.data(), globalVec4.second.size(), 0);
    for (const auto& globalMat4 : s_globalMat4)
        SetUniform(globalMat4.first, globalMat4.second.data(), globalMat4.second.size(), 0);
    for (const auto& globalMat3 : s_globalMat3)
        SetUniform(globalMat3.first, globalMat3.second.data(), globalMat3.second.size(), 0);
    for (const auto& globalFloat : s_globalFloats)
        SetUniform(globalFloat.first, globalFloat.second.data(), globalFloat.second.size(), 0);
    for (const auto& globalInt : s_globalInts)
        SetUniform(globalInt.first, globalInt.second.data(), globalInt.second.size(), 0);
    for (const auto& globalUint : s_globalUints)
        SetUniform(globalUint.first, globalUint.second.data(), globalUint.second.size(), 0);
    for (const auto& globalTexture : s_globalTextures)
        SetTexture(globalTexture.first, globalTexture.second);
}
void Program::Impl::UseNone()
{
    glUseProgram(0);
}
namespace Global {
    namespace Impl {
        void SetTexture(const std::string& name, const std::shared_ptr<Texture> value)
        {
            s_globalTextures[name] = value;
        }
        void SetUniform(const std::string& name, const float value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const float* value, const uint16_t count, const uint16_t index)
        {
            auto& values { s_globalFloats[name] };
            if (values.size() <= index + count)
                values.resize(index + count, 0);
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const int32_t value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const int32_t* value, const uint16_t count, const uint16_t index)
        {
            auto& values { s_globalInts[name] };
            if (values.size() <= index + count)
                values.resize(index + count, 0);
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const uint32_t value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const uint32_t* value, const uint16_t count, const uint16_t index)
        {
            auto& values { s_globalUints[name] };
            if (values.size() <= index + count)
                values.resize(index + count, 0);
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const glm::vec2& value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const glm::vec2* value, const uint16_t count, const uint16_t index)
        {
            auto& values{ s_globalVec2[name] };
            if (values.size() <= index + count)
                values.resize(index + count, glm::vec2(0));
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const glm::vec3& value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const glm::vec3* value, const uint16_t count, const uint16_t index)
        {
            auto& values{ s_globalVec3[name] };
            if (values.size() <= index + count)
                values.resize(index + count, glm::vec3(0));
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const glm::vec4& value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const glm::vec4* value, const uint16_t count, const uint16_t index)
        {
            auto& values{ s_globalVec4[name] };
            if (values.size() <= index + count)
                values.resize(index + count, glm::vec4(0));
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const glm::mat4& value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string& name, const glm::mat4* value, const uint16_t count, const uint16_t index)
        {
            auto& values{ s_globalMat4[name] };
            if (values.size() <= index + count)
                values.resize(index + count, glm::mat4(0));
            std::copy(value, value + count, values.begin() + index);
        }
        void SetUniform(const std::string& name, const glm::mat3& value)
        {
            SetUniform(name, &value, 1, 0);
        }
        void SetUniform(const std::string & name, const glm::mat3 * value, const uint16_t count, const uint16_t index)
        {
            auto& values{ s_globalMat3[name] };
            if (values.size() <= index + count)
                values.resize(index + count, glm::mat4(0));
            std::copy(value, value + count, values.begin() + index);
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