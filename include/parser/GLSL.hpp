/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:26:10
*/

#pragma once

#include "Shader.hpp"

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