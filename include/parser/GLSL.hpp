/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GLSL.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 17:12:33 by gpinchon          #+#    #+#             */
/*   Updated: 2018/11/15 16:17:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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