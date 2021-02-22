/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:48
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:43:25
*/

#include "Shader/Stage.hpp"

namespace Shader {
Stage::Stage(Stage::Type type, const Code& code)
{
    SetType(type);
    SetCode(code);
}

Stage::Stage(const Stage& other) : Stage(other.GetType(), other.GetCode())
{
    _defines = other._defines;
}

//WHY ?! WHY DO YOU NEED THIS ?!
Stage& Stage::operator=(const Stage& other)
{
    SetType(other.GetType());
    SetCode(other.GetCode());
    _defines = other._defines;
    return *this;
}

Stage& Shader::Stage::SetDefine(const std::string& name, const std::string& value)
{
    _defines[name] = value;
    return *this;
}

std::string Stage::GetDefine(const std::string& name) const
{
    auto value{ _defines.find(name) };
    if (value != _defines.end())
        return value->second;
    return "";
}

const std::map<std::string, std::string>& Stage::GetDefines() const
{
    return _defines;
}
Stage& Stage::operator+=(const Code& code)
{
    SetCode(GetCode() += code);
    return *this;
}
Stage& Stage::operator-=(const Code& code)
{
    SetCode(GetCode() -= code);
    return *this;
}
}

//#include "Debug.hpp"
//#include "Shader/Shader.hpp"
//
//ShaderStage::ShaderStage(GLenum stage, const std::shared_ptr<ShaderCode>& code)
//    : _stage(stage)
//{
//    AddExtension(code);
//}
//
//ShaderStage::~ShaderStage()
//{
//    Delete();
//}
//
//void ShaderStage::Delete()
//{
//    glDeleteShader(_glid);
//    _glid = 0;
//    _compiled = false;
//}
//
//void ShaderStage::Compile()
//{
//    if (Compiled()) {
//        Recompile();
//        return;
//    }
//    try {
//        static auto glslVersionString = std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
//        static auto glslVersionNbr = int(std::stof(glslVersionString) * 100);
//        _fullCode = std::string("#version ") + std::to_string(glslVersionNbr) + "\n";
//        for (auto define : _defines) {
//            _fullCode += "#define " + define.first + " " + define.second + "\n";
//        }
//        auto extensions = GetComponents<ShaderCode>();
//        for (const auto& extension : extensions) {
//            _fullCode += extension->Code() + '\n';
//        }
//        _fullCode += "void main(void) {\n";
//        for (const auto& extension : extensions) {
//            if (!extension->Technique().empty())
//                _fullCode += extension->Technique() + '\n';
//        }
//        _fullCode += "}\n";
//        auto codeBuff = _fullCode.c_str();
//        _glid = glCreateShader(Stage());
//        glShaderSource(_glid, 1, &codeBuff, nullptr);
//        glCompileShader(_glid);
//        Shader::check_shader(_glid);
//    } catch (std::exception& e) {
//        throw std::runtime_error(std::string("Error compiling Shader Stage ") + e.what());
//    }
//    _compiled = true;
//}
//
//void ShaderStage::Recompile()
//{
//    Delete();
//    Compile();
//}
//
//GLenum ShaderStage::Stage() const
//{
//    return _stage;
//}
//
///*void ShaderStage::AddExtension(const std::string& name, const std::shared_ptr<ShaderStage>& extension)
//{
//    if (_extensions[name]->Code() == extension->Code())
//        return;
//    _extensions[name] = extension;
//    _compiled = false;
//}*/
//
//void ShaderStage::AddExtension(const std::shared_ptr<ShaderCode>& extension)
//{
//    Component::AddComponent(extension);
//    _compiled = false;
//}
//
//void ShaderStage::RemoveExtension(const std::shared_ptr<ShaderCode>& extension)
//{
//    Component::RemoveComponent(extension);
//    _compiled = false;
//}
//
//void ShaderStage::RemoveExtension(const std::string& name)
//{
//}
//
///*std::shared_ptr<ShaderStage> ShaderStage::Extension(const std::string& name) const
//{
//    auto extension = _extensions.find(name);
//    if (extension == _extensions.end())
//        return nullptr;
//    return extension->second; 
//} */
//
//std::string ShaderStage::FullCode()
//{
//    return _fullCode;
//}
//
//bool ShaderStage::Compiled() const
//{
//    return _compiled;
//}
//
//void ShaderStage::SetDefine(const std::string define, const std::string value)
//{
//    _defines[define] = value;
//}
//
//void ShaderStage::RemoveDefine(const std::string define)
//{
//    _defines.erase(define);
//}
//
//GLuint ShaderStage::Glid() const
//{
//    return _glid;
//}
//
//std::string ShaderCode::Code() const
//{
//    return _code; //Le code c'est le _code ?
//}
//
//void ShaderCode::SetCode(const std::string& code)
//{
//    _code = code;
//    //_compiled = false;
//}
//
//std::string ShaderCode::Technique() const
//{
//    return _technique;
//}
//
//void ShaderCode::SetTechnique(const std::string technique)
//{
//    _technique = technique;
//}
