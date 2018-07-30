/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:12:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/30 19:48:19 by anonymous        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "vml.h"
#include <string>
#include "Object.hpp"

class Texture;
class Shader;

class	Material : public Object
{
public :
	static Material *create(const std::string &);
	static Material	*get_by_name(const std::string &);
	virtual void	bind_values();
	virtual void	bind_textures();
	virtual void	load_textures();
	Shader			*shader;
	VEC3			albedo{0, 0, 0};
	VEC3			specular{0, 0, 0};
	VEC3			emitting{0, 0, 0};
	VEC2			uv_scale{1, 1};
	float			alpha{1};
	float			parallax{0.05};
	Texture			*texture_albedo;
	Texture			*texture_specular;
	Texture			*texture_emitting;
	Texture			*texture_normal;
	Texture			*texture_height;
protected :
	Material(const std::string &name);
};