/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 11:58:26
*/

#include "parser/GLSL.hpp"
#include <GL/glew.h>                 // for GLuint, GL_FRAGMENT_SHADER, GL_V...
#include <bits/exception.h>          // for exception
#include <errno.h>                   // for errno
#include <io.h>                      // for access, R_OK
#include <string.h>                  // for strerror
#include <stdexcept>                 // for runtime_error
#include <unordered_map>             // for unordered_map
#include "Debug.hpp"                 // for glCheckError
#include "parser/InternalTools.hpp"  // for file_to_str

static auto deferredVertCode =
#include "deferred.vert"
    ;
static auto deferredFragCode =
#include "deferred.frag"
    ;
static auto forwardVertCode =
#include "forward.vert"
    ;
static auto forwardFragCode =
#include "forward.frag"
    ;

GLuint compile_shader_code(const std::string& code, GLenum type)
{
    GLuint shaderid;
    static auto glslVersionString = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    static auto glslVersionNbr = int(std::stof(glslVersionString) * 100);

    auto fullCode = std::string("#version ") + std::to_string(glslVersionNbr) + "\n" + code;
    auto codeBuff = fullCode.c_str();
    shaderid = glCreateShader(type);
    glShaderSource(shaderid, 1, &codeBuff, nullptr);
    glCompileShader(shaderid);
    glCheckError();
    Shader::check_shader(shaderid);
    return (shaderid);
}

GLuint compile_shader(const std::string& path, GLenum type)
{
    if (access(path.c_str(), R_OK) != 0) {
        throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
    }
    return (compile_shader_code(file_to_str(path), type));
}

std::shared_ptr<Shader> GLSL::compile(const std::string& name,
    const std::string& vertex_code, const std::string& fragment_code)
{
    auto shader = std::static_pointer_cast<GLSL>(Shader::create(name));
    GLuint vertexid = 0;
    GLuint fragmentid = 0;

    try {
        vertexid = compile_shader_code(vertex_code, GL_VERTEX_SHADER);
        fragmentid = compile_shader_code(fragment_code, GL_FRAGMENT_SHADER);
        shader->link(vertexid, fragmentid);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error compiling ") + name + " :\n" + e.what());
    }
    shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
    shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
    glDeleteShader(vertexid);
    glDeleteShader(fragmentid);
    glCheckError();
    return (shader);
}

std::shared_ptr<Shader> GLSL::parse(const std::string& name,
    const std::string& vertex_file_path,
    const std::string& fragment_file_path)
{
    return (GLSL::compile(name,
        file_to_str(vertex_file_path),
        file_to_str(fragment_file_path)));
}

std::shared_ptr<Shader> GLSL::compile(const std::string& name,
    const std::string& vertex_code, const std::string& fragment_code, ShaderType type, const std::string& defines)
{
    auto shader = std::static_pointer_cast<GLSL>(Shader::create(name));
    GLuint vertexid = 0;
    GLuint fragmentid = 0;
    try {
        if (ForwardShader == type) {
            vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + vertex_code, GL_VERTEX_SHADER);
            fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + fragment_code, GL_FRAGMENT_SHADER);
        } else if (LightingShader == type) {
            vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + vertex_code, GL_VERTEX_SHADER);
            fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + fragment_code, GL_FRAGMENT_SHADER);
        } else if (PostShader == type) {
            vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + vertex_code, GL_VERTEX_SHADER);
            fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + fragment_code, GL_FRAGMENT_SHADER);
        }
        shader->link(vertexid, fragmentid);
    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
    }
    shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
    shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
    glDeleteShader(vertexid);
    glDeleteShader(fragmentid);
    glCheckError();
    return (shader);
}

std::shared_ptr<Shader> GLSL::parse(const std::string& name,
    const std::string& vertex_file_path,
    const std::string& fragment_file_path,
    ShaderType type,
    const std::string& defines)
{
    auto vertex_code = file_to_str(vertex_file_path);
    auto fragment_code = file_to_str(fragment_file_path);
    return (GLSL::compile(name, vertex_code, fragment_code, type, defines));
}

std::shared_ptr<Shader> GLSL::compile(const std::string& name,
    const std::string& shader_code, ShaderType type, const std::string& defines)
{
    auto shader = std::static_pointer_cast<GLSL>(Shader::create(name));
    static auto emptyShaderCode =
#include "empty.glsl"
        ;

    GLuint vertexid = 0;
    GLuint fragmentid = 0;
    GLuint computeid = 0;
    try {
        if (ForwardShader == type) {
            vertexid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardVertCode + emptyShaderCode, GL_VERTEX_SHADER);
            fragmentid = compile_shader_code("#define FORWARDSHADER\n" + defines + forwardFragCode + shader_code, GL_FRAGMENT_SHADER);
            shader->link(vertexid, fragmentid);
        } else if (LightingShader == type) {
            vertexid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
            fragmentid = compile_shader_code("#define LIGHTSHADER\n" + defines + deferredFragCode + shader_code, GL_FRAGMENT_SHADER);
            shader->link(vertexid, fragmentid);
        } else if (PostShader == type) {
            vertexid = compile_shader_code("#define POSTSHADER\n" + defines + deferredVertCode + emptyShaderCode, GL_VERTEX_SHADER);
            fragmentid = compile_shader_code("#define POSTSHADER\n" + defines + deferredFragCode + shader_code, GL_FRAGMENT_SHADER);
            shader->link(vertexid, fragmentid);
        } else if (ComputeShader == type) {
            computeid = compile_shader_code(defines + shader_code, GL_COMPUTE_SHADER);
            shader->link(computeid);
        }

    } catch (std::exception& e) {
        throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
    }
    shader->_uniforms = shader->_get_variables(GL_ACTIVE_UNIFORMS);
    shader->_attributes = shader->_get_variables(GL_ACTIVE_ATTRIBUTES);
    glDeleteShader(vertexid);
    glDeleteShader(fragmentid);
    glDeleteShader(computeid);
    glCheckError();
    return (shader);
}

std::shared_ptr<Shader> GLSL::parse(const std::string& name,
    const std::string& shader_file_path, ShaderType type, const std::string& defines)
{
    auto shader_code = file_to_str(shader_file_path);
    return (GLSL::compile(name, shader_code, type, defines));
}
