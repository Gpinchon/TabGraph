/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:12:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/08 16:49:19 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "vml.h"
#include <string>

class Texture;
class Shader;

class	Material
{
public :
	static Material *create(const std::string &);
	static Material	*get_by_name(const std::string &);
	virtual void	bind_values();
	virtual void	bind_textures();
	virtual void	load_textures();
	void			set_name(const std::string &);
	const std::string		&name();
	Shader			*shader;
	VEC3			albedo{};
	VEC3			specular{};
	VEC3			emitting{};
	VEC2			uv_scale{};
	float			alpha{};
	float			parallax{};
	Texture			*texture_albedo;
	Texture			*texture_specular;
	Texture			*texture_emitting;
	Texture			*texture_normal;
	Texture			*texture_height;
protected :
	std::string		_name;
	size_t			_id{};
	Material(const std::string &name);
};