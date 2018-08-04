/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PBRMaterial.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:14:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/04 19:25:05 by gpinchon         ###   ########.fr       */
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
	Texture		*texture_roughness{nullptr};
	Texture		*texture_metallic{nullptr};
	Texture		*texture_ao{nullptr};
	//Texture		*texture_sss;
private :
	static Texture	*_texture_brdf;
	PBRMaterial(const std::string &name);
};