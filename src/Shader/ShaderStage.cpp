#include "Shader/ShaderStage.hpp"
#include "Shader/Shader.hpp"
#include "Debug.hpp"

static std::string empty_technique =
#include "empty.glsl"
    ;

ShaderStage::ShaderStage(GLenum stage, const std::string code) : _stage(stage), _code(code), _technique(empty_technique)
{
}

ShaderStage::~ShaderStage()
{
    Delete();
}

void ShaderStage::Delete() {
    glDeleteShader(_glid);
    _glid = 0;
    _compiled = false;
}

void ShaderStage::Compile()
{
    if (Compiled())
        Recompile();
    static auto glslVersionString = std::string((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    static auto glslVersionNbr = int(std::stof(glslVersionString) * 100);
    _fullCode = std::string("#version ") + std::to_string(glslVersionNbr) + "\n";
    for (auto define : _defines) {
        _fullCode += "#define " + define.first + " " + define.second + "\n";
    }
    _fullCode += Code() + Technique();
    auto codeBuff = _fullCode.c_str();
    _glid = glCreateShader(Stage());
    glShaderSource(_glid, 1, &codeBuff, nullptr);
    glCompileShader(_glid);
    glCheckError();
    try {
        Shader::check_shader(_glid);
    }
    catch (std::exception& e) {
        throw std::runtime_error(std::string("Error compiling Shader Stage ") + e.what());
    }
    _compiled = true;
}

void ShaderStage::Recompile()
{
    Delete();
    Compile();
}

GLenum ShaderStage::Stage() const
{
    return _stage;
}

std::string ShaderStage::FullCode()
{
    return _fullCode;
}

std::string ShaderStage::Code() const
{
    return _code; //Le code c'est le _code ?
}

bool ShaderStage::Compiled() const
{
    return _compiled;
}

void ShaderStage::SetDefine(const std::string define, const std::string value)
{
    _defines[define] = value;
}

void ShaderStage::RemoveDefine(const std::string define)
{
    _defines.erase(define);
}

std::string ShaderStage::Technique() const
{
    return _technique;
}

void ShaderStage::SetTechnique(const std::string technique)
{
    _technique = technique;
}

GLuint ShaderStage::Glid() const
{
    return _glid;
}
