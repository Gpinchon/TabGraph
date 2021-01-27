/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:19
*/

#pragma once

#include "Shader/Shader.hpp"
#include <memory> // for shared_ptr, allocator
#include <string> // for string

/*
** .GLSL parsing interface
*/
namespace GLSL {
/*static std::shared_ptr<Shader> compile(const std::string& name,
        const std::string& vertex_code, const std::string& fragment_code);
    static std::shared_ptr<Shader> compile(const std::string& name,
        const std::string& vertex_code, const std::string& fragment_code, ShaderType type, const std::string& defines = "");
    static std::shared_ptr<Shader> compile(const std::string& name,
        const std::string& fragment_code, ShaderType type, const std::string& defines = "");*/

std::shared_ptr<Shader> parse(const std::string& name,
    const std::string& vertex_file_path, const std::string& fragment_file_path);
std::shared_ptr<Shader> parse(const std::string& name,
    const std::string& vertex_file_path, const std::string& fragment_file_path, Shader::Type type, const std::string& defines = "");
std::shared_ptr<Shader> parse(const std::string& name,
    const std::string& fragment_file_path, Shader::Type type, const std::string& defines = "");
};