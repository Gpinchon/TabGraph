/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:12:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/03 23:08:28 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Object.hpp"
#include <string>

class Texture;
class Framebuffer;
class Shader;

class	Material : public Object
{
public :
	static Material *create(const std::string &);
	static Material	*get_by_name(const std::string &);
	virtual void	bind_values();
	virtual void	bind_textures();
	Shader			*shader{nullptr};
	Shader			*depth_shader{nullptr};
	VEC3			albedo{0, 0, 0};
	VEC3			specular{0.04, 0.04, 0.04};
	VEC3			emitting{0, 0, 0};
	VEC2			uv_scale{1, 1};
	float			roughness{0.5};
	float			metallic{0};
	float			alpha{1};
	float			parallax{0.01};
	float			ior{1};
	Texture			*texture_albedo{nullptr};
	Texture			*texture_specular{nullptr};
	Texture			*texture_emitting{nullptr};
	Texture			*texture_normal{nullptr};
	Texture			*texture_height{nullptr};
	Texture			*texture_roughness{nullptr};
	Texture			*texture_metallic{nullptr};
	Texture			*texture_ao{nullptr};
protected :
	static Texture	*_texture_brdf;
	Material(const std::string &name);
};