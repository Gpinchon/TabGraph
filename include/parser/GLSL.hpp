/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GLSL.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 17:12:33 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/06 15:56:39 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Shader.hpp"

enum ShaderType
{
	ForwardShader, LightingShader, PostShader
};

/*
** .GLSL parsing interface
*/
class	GLSL : public Shader
{
public:
	static Shader	*parse(const std::string &name, const std::string &vertex_file_path,
						const std::string &fragment_file_path, ShaderType type);
	static Shader	*parse(const std::string &name, const std::string &vertex_file_path,
						const std::string &fragment_file_path);
	static Shader	*parse(const std::string &name, const std::string &fragment_file_path, ShaderType type);
private:
	virtual void abstract() = 0;
};