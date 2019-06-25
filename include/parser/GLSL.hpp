/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-25 11:58:12
*/

#pragma once

#include <memory>      // for shared_ptr, allocator
#include <string>      // for string
#include "Shader.hpp"  // for Shader

enum ShaderType {
    ForwardShader,
    LightingShader,
    PostShader,
    ComputeShader
};

/*
** .GLSL parsing interface
*/
class GLSL : public Shader {
public:
    static std::shared_ptr<Shader> compile(const std::string& name,
        const std::string& vertex_code, const std::string& fragment_code);
    static std::shared_ptr<Shader> compile(const std::string& name,
        const std::string& vertex_code, const std::string& fragment_code, ShaderType type, const std::string& defines = "");
    static std::shared_ptr<Shader> compile(const std::string& name,
        const std::string& fragment_code, ShaderType type, const std::string& defines = "");

    static std::shared_ptr<Shader> parse(const std::string& name,
        const std::string& vertex_file_path, const std::string& fragment_file_path);
    static std::shared_ptr<Shader> parse(const std::string& name,
        const std::string& vertex_file_path, const std::string& fragment_file_path, ShaderType type, const std::string& defines = "");
    static std::shared_ptr<Shader> parse(const std::string& name,
        const std::string& fragment_file_path, ShaderType type, const std::string& defines = "");

private:
    virtual void abstract() = 0;
};