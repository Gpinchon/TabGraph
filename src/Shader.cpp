/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:07:09
*/

#include "Shader.hpp"
#include "Debug.hpp"
#include "Engine.hpp"
#include "Texture.hpp"

std::vector<std::shared_ptr<Shader>> Shader::_shaders;

Shader::Shader(const std::string& name)
    : Object(name)
{
}

std::shared_ptr<Shader> Shader::create(const std::string& name)
{
    auto shader = std::shared_ptr<Shader>(new Shader(name));
    _shaders.push_back(shader);
    return (shader);
}

std::shared_ptr<Shader> Shader::get(unsigned index)
{
    if (index >= _shaders.size())
        return (nullptr);
    return (_shaders.at(index));
}

std::shared_ptr<Shader> Shader::get_by_name(const std::string& name)
{
    for (auto s : _shaders) {
        if (name == s->name())
            return (s);
    }
    return (nullptr);
}

ShaderVariable* Shader::get_uniform(const std::string& name)
{
    auto it = _uniforms.find(name);
    if (it != _uniforms.end()) {
        return (&it->second);
    }
    return (nullptr);
}

void Shader::set_uniform(const std::string& name, const int& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glUniform1iv(v->loc, nbr, &value);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::set_uniform(const std::string& name, const bool& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    int val = value;
    glUniform1iv(v->loc, nbr, &val);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::set_uniform(const std::string& name, const unsigned& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glUniform1uiv(v->loc, nbr, &value);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::set_uniform(const std::string& name, const float& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glUniform1fv(v->loc, nbr, &value);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::set_uniform(const std::string& name, const VEC2& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glUniform2fv(v->loc, nbr, &value.x);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::set_uniform(const std::string& name, const VEC3& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glUniform3fv(v->loc, nbr, &value.x);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

void Shader::set_uniform(const std::string& name, const MAT4& value, unsigned nbr)
{
    auto v = get_uniform(name);
    if (v == nullptr) {
        return;
    }
    bool bound = in_use();
    if (!bound) {
        use();
    }
    glUniformMatrix4fv(v->loc, nbr, GL_FALSE, (float*)&value);
    glCheckError();
    if (!bound) {
        use(false);
    }
}

bool Shader::in_use()
{
    return (_in_use);
}

void Shader::use(const bool& use_program)
{
    if (!use_program) {
        glUseProgram(0);
        return;
    }
    glUseProgram(_program);
    if (glCheckError()) {
        debugLog(name());
    }
    _in_use = true;
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
            layer, access, texture->internal_format());
        glCheckError();
        //glBindTexture(texture->target(), texture->glid());
    }
    set_uniform(name, int(texture_unit - GL_TEXTURE0));
    if (!bound) {
        use(false);
    }
}

void Shader::bind_texture(const std::string& name,
    std::shared_ptr<Texture> texture, const GLenum texture_unit)
{
    bool bound = in_use();
    if (!bound) {
        use();
    }
    if (texture == nullptr) {
        unbind_texture(texture_unit);
    } else {
        texture->load();
        glActiveTexture(texture_unit);
        glBindTexture(texture->target(), texture->glid());
        glCheckError();
    }
    set_uniform(name, int(texture_unit - GL_TEXTURE0));
    if (!bound) {
        use(false);
    }
}

GLuint Shader::link(const GLuint shaderid)
{
    if (_program == 0)
        _program = glCreateProgram();
    glAttachShader(_program, shaderid);
    glLinkProgram(_program);
    glDetachShader(_program, shaderid);
    glCheckError();
    try {
        check_program(_program);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Linking Error :\n") + e.what());
    }
    return (_program);
}

void Shader::attach(const GLuint shaderid)
{
    if (_program == 0)
        _program = glCreateProgram();
    glAttachShader(_program, shaderid);
    glCheckError();
}

void Shader::detach(const GLuint shaderid)
{
    if (_program == 0)
        _program = glCreateProgram();
    glDetachShader(_program, shaderid);
    glCheckError();
}

GLuint Shader::link()
{
    glLinkProgram(_program);
    glCheckError();
    try {
        check_program(_program);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Linking Error :\n") + e.what());
    }
    return (_program);
}

GLuint Shader::link(const GLuint vertexid, const GLuint fragmentid)
{
    if (_program == 0)
        _program = glCreateProgram();
    glAttachShader(_program, vertexid);
    glAttachShader(_program, fragmentid);
    glLinkProgram(_program);
    glDetachShader(_program, vertexid);
    glDetachShader(_program, fragmentid);
    glCheckError();
    try {
        check_program(_program);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Linking Error :\n") + e.what());
    }
    return (_program);
}

GLuint Shader::link(const GLuint geometryid, const GLuint vertexid, const GLuint fragmentid)
{
    if (_program == 0)
        _program = glCreateProgram();
    glAttachShader(_program, geometryid);
    glAttachShader(_program, vertexid);
    glAttachShader(_program, fragmentid);
    glLinkProgram(_program);
    glDetachShader(_program, geometryid);
    glDetachShader(_program, vertexid);
    glDetachShader(_program, fragmentid);
    glCheckError();
    try {
        check_program(_program);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Linking Error :\n") + e.what());
    }
    return (_program);
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

std::unordered_map<std::string, ShaderVariable> Shader::_get_variables(GLenum type)
{
    char name[4096];
    GLint ivcount;
    GLsizei length;

    glGetProgramiv(_program, type, &ivcount);
    std::unordered_map<std::string, ShaderVariable> variables;
    while (--ivcount >= 0) {
        ShaderVariable v;
        memset(name, 0, sizeof(char) * 4096);
        glGetActiveUniform(_program, static_cast<GLuint>(ivcount), 4096, &length,
            &v.size, &v.type, name);
        v.name = name;
        std::hash<std::string> hash_fn;
        v.id = hash_fn(name);
        v.loc = glGetUniformLocation(_program, name);
        variables.insert(std::make_pair(v.name, v));
    }
    glCheckError();
    return (variables);
}
