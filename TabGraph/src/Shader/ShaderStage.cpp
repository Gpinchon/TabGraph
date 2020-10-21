#include "Shader/ShaderStage.hpp"
#include "Debug.hpp"
#include "Shader/Shader.hpp"

/*static std::string empty_technique =
#include "empty.glsl"
    ;*/

ShaderStage::ShaderStage(GLenum stage, const std::shared_ptr<ShaderCode>& code)
    : _stage(stage)
{
    AddExtension(code);
}

std::shared_ptr<ShaderStage> ShaderStage::Create(GLenum stage, const std::shared_ptr<ShaderCode>& code)
{
    return std::shared_ptr<ShaderStage>(new ShaderStage(stage, code));
}

ShaderStage::~ShaderStage()
{
    Delete();
}

void ShaderStage::Delete()
{
    glDeleteShader(_glid);
    _glid = 0;
    _compiled = false;
}

void ShaderStage::Compile()
{
    if (Compiled())
        Recompile();
    try {
        static auto glslVersionString = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
        static auto glslVersionNbr = int(std::stof(glslVersionString) * 100);
        _fullCode = std::string("#version ") + std::to_string(glslVersionNbr) + "\n";
        for (auto define : _defines) {
            _fullCode += "#define " + define.first + " " + define.second + "\n";
        }
        //_fullCode += Code();
        auto extensions = GetComponents<ShaderCode>();
        for (const auto& extension : extensions) {
            _fullCode += extension->Code() + '\n';
            //{
            //    std::string str = "/*EXTENSIONS_CODE*/";
            //    auto pos = _fullCode.find(str);
            //    _fullCode.insert(pos + str.size(), extension.second->Code());
            //}
        }
        _fullCode += "void main() {\n";
        /*if (!Technique().empty())
            _fullCode += Technique() + "();\n";*/
        for (const auto& extension : extensions) {
            if (!extension->Technique().empty())
                _fullCode += extension->Technique() + '\n';
            //{
            //    std::string str = "/*EXTENSIONS_FUNCTIONS*/";
            //    auto pos = _fullCode.find(str);
            //    _fullCode.insert(pos + str.size(), extension.first + "();");
            //}
        }
        _fullCode += "}\n";
        auto codeBuff = _fullCode.c_str();
        _glid = glCreateShader(Stage());
        glShaderSource(_glid, 1, &codeBuff, nullptr);
        glCompileShader(_glid);
        Shader::check_shader(_glid);
    } catch (std::exception& e) {
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

/*void ShaderStage::AddExtension(const std::string& name, const std::shared_ptr<ShaderStage>& extension)
{
    if (_extensions[name]->Code() == extension->Code())
        return;
    _extensions[name] = extension;
    _compiled = false;
}*/

void ShaderStage::AddExtension(const std::shared_ptr<ShaderCode>& extension)
{
    Component::AddComponent(extension);
    _compiled = false;
}

void ShaderStage::RemoveExtension(const std::string& name)
{
}

/*std::shared_ptr<ShaderStage> ShaderStage::Extension(const std::string& name) const
{
    auto extension = _extensions.find(name);
    if (extension == _extensions.end())
        return nullptr;
    return extension->second; 
} */

std::string ShaderStage::FullCode()
{
    return _fullCode;
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

GLuint ShaderStage::Glid() const
{
    return _glid;
}

std::shared_ptr<ShaderCode> ShaderCode::Create(const std::string& code, const std::string& technique)
{
    return std::shared_ptr<ShaderCode>(new ShaderCode(code, technique));
}

std::string ShaderCode::Code() const
{
    return _code; //Le code c'est le _code ?
}

void ShaderCode::SetCode(const std::string& code)
{
    _code = code;
    //_compiled = false;
}

std::string ShaderCode::Technique() const
{
    return _technique;
}

void ShaderCode::SetTechnique(const std::string technique)
{
    _technique = technique;
}
