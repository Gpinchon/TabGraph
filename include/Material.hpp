/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:12:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 23:03:26 by gpinchon         ###   ########.fr       */
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
	VEC3			albedo;
	VEC3			emitting;
	VEC2			uv_scale;
	float			alpha;
	Texture			*texture_albedo;
protected :
	static Texture	*_texture_brdf;
	std::string		_name;
	size_t			_id;
	Material(const std::string &name);
};