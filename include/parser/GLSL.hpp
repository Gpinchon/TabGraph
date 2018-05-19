/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   GLSL.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 17:12:33 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 20:21:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Shader.hpp"

/*
** .GLSL parsing interface
*/
class	GLSL : public Shader
{
public:
	static Shader	*parse(const std::string &name, const std::string &vertex_file_path,
						const std::string &fragment_file_path);
private:
	virtual void abstract() = 0;
};