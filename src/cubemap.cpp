/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cubemap.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 16:36:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 01:01:27 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

void	cubemap_load_side(Cubemap *texture, const std::string &path, GLenum side)
{
	auto	sideTexture = BMP::parse(path, path);
	if (!sideTexture)
		return ;
	texture->assign(*sideTexture, side);
	//texture->sides[GL_TEXTURE_CUBE_MAP_POSITIVE_X - side] = sideTexture;
}

Cubemap		*Cubemap::parse(const std::string &name, const std::string &path)
{
	auto	t = static_cast<Cubemap *>(Texture::create(name, new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0));
	cubemap_load_side(t, path + name + "/X+.bmp", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	cubemap_load_side(t, path + name + "/X-.bmp", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	cubemap_load_side(t, path + name + "/Y-.bmp", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	cubemap_load_side(t, path + name + "/Y+.bmp", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	cubemap_load_side(t, path + name + "/Z+.bmp", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	cubemap_load_side(t, path + name + "/Z-.bmp", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	t->generate_mipmap();
	Engine::add(*t);
	return (t);
}
