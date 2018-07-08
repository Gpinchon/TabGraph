/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PBRMaterial.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:14:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/08 16:47:58 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Material.hpp"

struct	PBRMaterial : public Material
{
	static PBRMaterial *create(const std::string &);
	void	bind_values();
	void	bind_textures();
	void	load_textures();
	float		roughness{};
	float		metallic{};
	Texture		*texture_roughness;
	Texture		*texture_metallic;
	Texture		*texture_ao;
private :
	static Texture	*_texture_brdf;
	PBRMaterial(const std::string &name);
};