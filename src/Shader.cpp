/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 12:18:05
*/

#include "Debug.hpp" // for glCheckError, debugLog
#include "GLUniformHelper.hpp"
#include "Shader.hpp"
#include "Texture.hpp" // for Texture
#include <bits/exception.h> // for exception
#include <stdexcept> // for runtime_error
#include <string.h> // for memset
#include <utility> // for pair, make_pair

static std::string forward_vert_code =
#include "forward.vert"
    ;

static std::string forward_frag_code =
#include "forward.frag"
    ;

static std::string deferred_vert_code =
#include "deferred.vert"
    ;
static std::string deferred_frag_code =
#include "deferred.frag"
    ;

std::vector<std::shared_ptr<Shader>> Shader::_shaders;

Shader::Shader(const std::string& name)
    : Object(name)
{
}

std::shared_ptr<Shader> Shader::Create(const std::string& name, ShaderType type)
{
    auto shader = std::shared_ptr<Shader>(new Shader(name));
    shader->_type = type;
    if (ForwardShader == type) {
        shader->SetDefine("FORWARDSHADER");
        shader->SetStage(ShaderStage(GL_VERTEX_SHADER, forward_vert_code));
        shader->SetStage(ShaderStage(GL_FRAGMENT_SHADER, forward_frag_code));
    }
    else if (LightingShader == type) {
        shader->SetDefine("LIGHTSHADER");
        shader->SetStage(ShaderStage(GL_VERTEX_SHADER, deferred_vert_code));
        shader->SetStage(ShaderStage(GL_FRAGMENT_SHADER, deferred_frag_code));
    }
    else if (PostShader == type) {
        shader->SetDefine("POSTSHADER");
        shader->SetStage(ShaderStage(GL_VERTEX_SHADER, deferred_vert_code));
        shader->SetStage(ShaderStage(GL_FRAGMENT_SHADER, deferred_frag_code));
    }
    else if (ComputeShader == type) {
        shader->SetDefine("COMPUTESHADER");
    }
    _shaders.push_back(shader);
    return (shader);
}

std::shared_ptr<Shader> Shader::Get(unsigned index)
{
    if (index >= _shaders.size())
        return (nullptr);
    return (_shaders.at(index));
}

std::shared_ptr<Shader> Shader::GetByName(const std::string& name)
{
    for (auto s : _shaders) {
        if (name == s->Name())
            return (s);
    }
    return (nullptr);
}

ShaderVariable &Shader::get_attribute(const std::string& name)
{
    return _attributes[name];
}

ShaderVariable &Shader::get_uniform(const std::string& name)
{
    return _uniforms[name];
}

bool Shader::in_use()
{
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    return (_program != 0 && GLuint(program) == _program);
}

void Shader::use(const bool& use_program)
{
    if (!use_program) {
        glUseProgram(0);
        return;
    }
    if (NeedsRecompile())
        Recompile();
    Compile();
    glUseProgram(_program);
    glCheckError();
    _UpdateVariables();
    if (glCheckError()) {
        debugLog(Name());
    }
}

void Shader::unbind_texture(GLenum texture_unit)
{
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glActiveTexture(texture_unit);
    glBindTexture(GL_TEXTURE_2D, 0);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::bind_image(const std::string& name,
    std::shared_ptr<Texture> texture, const GLint level, const bool layered, const GLint layer, const GLenum access, const GLenum texture_unit)
{
    bool bound = in_use();
    if (!bound) {
        use();
    }
    if (texture == nullptr) {
        unbind_texture(texture_unit);
    } else {
        texture->load();
        //glActiveTexture(texture_unit);
        glBindTexture(texture->target(), texture->glid());
        glBindImageTexture(texture_unit - GL_TEXTURE0,
            texture->glid(), level, layered,
            layer, access, texture->InternalFormat());
        glCheckError();
        //glBindTexture(texture->target(), texture->glid());
    }
    SetUniform(name, int(texture_unit - GL_TEXTURE0));
    if (!bound) {
        use(false);
    }
}

void Shader::_UpdateVariable(const ShaderVariable &variable)
{
    if (variable.loc == -1)
        return;
    variable.updateFunction(variable);
}

void Shader::_UpdateVariables()
{
    if (_uniformsChanged) {
        for (const auto & uniform : _uniforms)
        {
            auto v(uniform.second);
            _UpdateVariable(v);
        }
        _uniformsChanged = false;
    }
    if (_attributesChanged) {
        for (const auto & uniform : _uniforms)
        {
            auto v(uniform.second);
            _UpdateVariable(v);
        }
        _attributesChanged = false;
    }
}

void Shader::Link()
{
    glLinkProgram(_program);
    glCheckError();
    try {
        check_program(_program);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Linking Error " + Name() + " :\n") + e.what());
    }
    _get_variables(GL_ACTIVE_UNIFORMS);
    _get_variables(GL_ACTIVE_ATTRIBUTES);
}

bool Shader::check_shader(const GLuint id)
{
    GLint result;
    GLint loglength;

    result = GL_FALSE;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result != GL_TRUE) {
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &loglength);
        if (loglength > 1) {
            char log[loglength];
            glGetShaderInfoLog(id, loglength, nullptr, &log[0]);
            throw std::runtime_error(log);
        } else {
            throw std::runtime_error("Unknown Error");
        }
    }
    glCheckError();
    return (false);
}

bool Shader::check_program(const GLuint id)
{
    GLint result;
    GLint loglength;

    result = GL_FALSE;
    glGetProgramiv(id, GL_LINK_STATUS, &result);
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &loglength);
    if (loglength > 1) {
        char log[loglength];
        glGetProgramInfoLog(id, loglength, nullptr, &log[0]);
        throw std::runtime_error(log);
    }
    glCheckError();
    return (false);
}

static inline size_t VariableSize(GLenum type)
{
    switch (type) {
        case(GL_FLOAT) :
            return sizeof(float);
        case(GL_FLOAT_VEC2) :
            return sizeof(glm::vec2);
        case(GL_FLOAT_VEC3) :
            return sizeof(glm::vec3);
        case(GL_FLOAT_VEC4) :
            return sizeof(glm::vec4);
        case(GL_DOUBLE) :
            return sizeof(double);
        case(GL_DOUBLE_VEC2) :
            return sizeof(glm::dvec2);
        case(GL_DOUBLE_VEC3) :
            return sizeof(glm::dvec3);
        case(GL_DOUBLE_VEC4) :
            return sizeof(glm::dvec4);
        case(GL_INT) :
            return sizeof(int);
        case(GL_INT_VEC2) :
            return sizeof(glm::ivec2);
        case(GL_INT_VEC3) :
            return sizeof(glm::ivec3);
        case(GL_INT_VEC4) :
            return sizeof(glm::ivec4);
        case(GL_UNSIGNED_INT) :
            return sizeof(unsigned);
        case(GL_UNSIGNED_INT_VEC2) :
            return sizeof(glm::uvec2);
        case(GL_UNSIGNED_INT_VEC3) :
            return sizeof(glm::uvec3);
        case(GL_UNSIGNED_INT_VEC4) :
            return sizeof(glm::uvec4);
        case(GL_BOOL) :
            return sizeof(bool);
        case(GL_BOOL_VEC2) :
            return sizeof(glm::bvec2);
        case(GL_BOOL_VEC3) :
            return sizeof(glm::bvec3);
        case(GL_BOOL_VEC4) :
            return sizeof(glm::bvec4);
        case(GL_FLOAT_MAT2) :
            return sizeof(glm::mat2);
        case(GL_FLOAT_MAT3) :
            return sizeof(glm::mat3);
        case(GL_FLOAT_MAT4) :
            return sizeof(glm::mat4);
        case(GL_FLOAT_MAT2x3) :
            return sizeof(glm::mat2x3);
        case(GL_FLOAT_MAT2x4) :
            return sizeof(glm::mat2x4);
        case(GL_FLOAT_MAT3x2) :
            return sizeof(glm::mat3x2);
        case(GL_FLOAT_MAT3x4) :
            return sizeof(glm::mat3x4);
        case(GL_FLOAT_MAT4x2) :
            return sizeof(glm::mat4x2);
        case(GL_FLOAT_MAT4x3) :
            return sizeof(glm::mat4x3);
        case(GL_DOUBLE_MAT2) :
            return sizeof(glm::dmat2);
        case(GL_DOUBLE_MAT3) :
            return sizeof(glm::dmat3);
        case(GL_DOUBLE_MAT4) :
            return sizeof(glm::dmat4);
        case(GL_DOUBLE_MAT2x3) :
            return sizeof(glm::dmat2x3);
        case(GL_DOUBLE_MAT2x4) :
            return sizeof(glm::dmat2x4);
        case(GL_DOUBLE_MAT3x2) :
            return sizeof(glm::dmat3x2);
        case(GL_DOUBLE_MAT3x4) :
            return sizeof(glm::dmat3x4);
        case(GL_DOUBLE_MAT4x2) :
            return sizeof(glm::dmat4x2);
        case(GL_DOUBLE_MAT4x3) :
            return sizeof(glm::dmat4x3);
        case(GL_UNSIGNED_INT_ATOMIC_COUNTER) :
            return sizeof(unsigned);
        case (GL_SAMPLER_1D) :
        case (GL_SAMPLER_2D) :
        case (GL_SAMPLER_3D) :
        case (GL_SAMPLER_CUBE) :
        case (GL_SAMPLER_1D_SHADOW) :
        case (GL_SAMPLER_2D_SHADOW) :
        case (GL_SAMPLER_1D_ARRAY) :
        case (GL_SAMPLER_2D_ARRAY) :
        case (GL_SAMPLER_1D_ARRAY_SHADOW) :
        case (GL_SAMPLER_2D_ARRAY_SHADOW) :
        case (GL_SAMPLER_2D_MULTISAMPLE) :
        case (GL_SAMPLER_2D_MULTISAMPLE_ARRAY) :
        case (GL_SAMPLER_CUBE_SHADOW) :
        case (GL_SAMPLER_BUFFER) :
        case (GL_SAMPLER_2D_RECT) :
        case (GL_SAMPLER_2D_RECT_SHADOW) :
        case (GL_INT_SAMPLER_1D) :
        case (GL_INT_SAMPLER_2D) :
        case (GL_INT_SAMPLER_3D) :
        case (GL_INT_SAMPLER_CUBE) :
        case (GL_INT_SAMPLER_1D_ARRAY) :
        case (GL_INT_SAMPLER_2D_ARRAY) :
        case (GL_INT_SAMPLER_2D_MULTISAMPLE) :
        case (GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) :
        case (GL_INT_SAMPLER_BUFFER) :
        case (GL_INT_SAMPLER_2D_RECT) :
        case (GL_UNSIGNED_INT_SAMPLER_1D) :
        case (GL_UNSIGNED_INT_SAMPLER_2D) :
        case (GL_UNSIGNED_INT_SAMPLER_3D) :
        case (GL_UNSIGNED_INT_SAMPLER_CUBE) :
        case (GL_UNSIGNED_INT_SAMPLER_1D_ARRAY) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_ARRAY) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY) :
        case (GL_UNSIGNED_INT_SAMPLER_BUFFER) :
        case (GL_UNSIGNED_INT_SAMPLER_2D_RECT) :
        case (GL_IMAGE_1D) :
        case (GL_IMAGE_2D) :
        case (GL_IMAGE_3D) :
        case (GL_IMAGE_2D_RECT) :
        case (GL_IMAGE_CUBE) :
        case (GL_IMAGE_BUFFER) :
        case (GL_IMAGE_1D_ARRAY) :
        case (GL_IMAGE_2D_ARRAY) :
        case (GL_IMAGE_2D_MULTISAMPLE) :
        case (GL_IMAGE_2D_MULTISAMPLE_ARRAY) :
        case (GL_INT_IMAGE_1D) :
        case (GL_INT_IMAGE_2D) :
        case (GL_INT_IMAGE_3D) :
        case (GL_INT_IMAGE_2D_RECT) :
        case (GL_INT_IMAGE_CUBE) :
        case (GL_INT_IMAGE_BUFFER) :
        case (GL_INT_IMAGE_1D_ARRAY) :
        case (GL_INT_IMAGE_2D_ARRAY) :
        case (GL_INT_IMAGE_2D_MULTISAMPLE) :
        case (GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY) :
        case (GL_UNSIGNED_INT_IMAGE_1D) :
        case (GL_UNSIGNED_INT_IMAGE_2D) :
        case (GL_UNSIGNED_INT_IMAGE_3D) :
        case (GL_UNSIGNED_INT_IMAGE_2D_RECT) :
        case (GL_UNSIGNED_INT_IMAGE_CUBE) :
        case (GL_UNSIGNED_INT_IMAGE_BUFFER) :
        case (GL_UNSIGNED_INT_IMAGE_1D_ARRAY) :
        case (GL_UNSIGNED_INT_IMAGE_2D_ARRAY) :
        case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE) :
        case (GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY) :
            return sizeof(std::pair<std::shared_ptr<Texture>, GLenum>);
    }
    return (0);
}

void Shader::_get_variables(GLenum variableType)
{
    char name[4096];
    GLint ivcount;
    GLsizei length;

    glGetProgramiv(_program, variableType, &ivcount);
    glCheckError();
    debugLog(this->Name());
    debugLog(ivcount);
    debugLog((variableType == GL_ACTIVE_UNIFORMS ? "GL_ACTIVE_UNIFORMS" : "GL_ACTIVE_ATTRIBUTES"));
    while (--ivcount >= 0) {
        memset(name, 0, sizeof(name));
        GLint size;
        GLenum type;
        glGetActiveUniform(_program, static_cast<GLuint>(ivcount), 4096, &length, &size, &type, name);
        debugLog(name);
        glCheckError();
        auto &v(variableType == GL_ACTIVE_UNIFORMS ? _uniforms[name] : _attributes[name]);
        v.name = name;
        v.size = size;
        v.type = type;
        v.loc = glGetUniformLocation(_program, name);
        v.byteSize = VariableSize(v.type);
        //v.data = (void*)nullptr;
        v.updateFunction = GetSetUniformCallback(v.type);
        if (variableType == GL_ACTIVE_UNIFORMS)
            _uniforms[name] = v;
        else if (variableType == GL_ACTIVE_ATTRIBUTES)
            _attributes[name] = v;
        debugLog(v.name + " " + std::to_string(v.size) + " " + std::to_string(v.type) + " " + std::to_string(v.loc));
        glCheckError();
    }
}

void Shader::Compile()
{
    if (Compiled())
        return;
    _program = glCreateProgram();
    for (auto &stagePair : _shaderStages) {
        auto stage(stagePair.second);
        for (auto define : _defines) {
            debugLog(define.first + " " + define.second);
            stage.SetDefine(define.first, define.second);
        }
        try {
            stage.Compile();
        }
        catch (std::exception& e) {
            throw std::runtime_error(std::string("Error compiling ") + Name() + "\n" + e.what() +
                "\nShader Code :\n" + stage.Code());
        }
        glAttachShader(_program, stage.Glid());
    }
    Link();
    _compiled = true;
}

void Shader::Recompile()
{
    glDeleteProgram(_program);
    _program = 0;
    _compiled = false;
    for (auto &stagePair : _shaderStages)
        stagePair.second.Delete();
    Compile();
    _needsRecompile = false;
}

bool Shader::NeedsRecompile() const
{
    return _needsRecompile;
}

void Shader::SetDefine(const std::string define, const std::string value)
{
    if (_defines.find(define) == _defines.end() || _defines[define] != value)
        _needsRecompile = true;
    _defines[define] = value;
}

void Shader::RemoveDefine(const std::string define)
{
    for (auto &stagePair : _shaderStages)
        stagePair.second.RemoveDefine(define);
    if (_defines.find(define) != _defines.end())
        _needsRecompile = true;
    _defines.erase(define);
}

ShaderStage &Shader::Stage(GLenum stage)
{
    return _shaderStages[stage];
}

void Shader::SetStage(const ShaderStage &stage)
{
    _shaderStages[stage.Stage()] = stage;
}

bool Shader::Compiled() const
{
    return _compiled;
}
