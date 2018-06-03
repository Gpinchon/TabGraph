/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cubemap.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 16:36:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/06/03 18:28:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Cubemap.hpp"
#include "parser/BMP.hpp"

void	cubemap_load_side(Cubemap *texture, const std::string &path, GLenum side)
{
	auto	sideTexture = BMP::parse(path, path);
	if (sideTexture == nullptr){
		return ;
	}
	texture->assign(*sideTexture, side);
	texture->sides.at(side - GL_TEXTURE_CUBE_MAP_POSITIVE_X) = sideTexture;
}

Cubemap::Cubemap(const std::string &name) : Texture(name)
{
	//memset(sides, 0, sizeof(sides));
	_target = GL_TEXTURE_CUBE_MAP;
	glGenTextures(1, &_glid);
	glBindTexture(_target, _glid);
	glTexParameteri(_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glBindTexture(_target, 0);
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, _glid, -1, name.c_str());
#endif //GL_DEBUG
}

Cubemap		*Cubemap::create(const std::string &name)
{
	auto	cubemap = new Cubemap(name);
	Engine::add(*cubemap);
	return (cubemap);
}

Cubemap		*Cubemap::parse(const std::string &name, const std::string &path)
{
	try {
		auto	t = Cubemap::create(name);
		cubemap_load_side(t, path + name + "/X+.bmp", GL_TEXTURE_CUBE_MAP_POSITIVE_X);
		cubemap_load_side(t, path + name + "/X-.bmp", GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
		cubemap_load_side(t, path + name + "/Y-.bmp", GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
		cubemap_load_side(t, path + name + "/Y+.bmp", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
		cubemap_load_side(t, path + name + "/Z+.bmp", GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
		cubemap_load_side(t, path + name + "/Z-.bmp", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
		t->generate_mipmap();
		return (t);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing Cubemap : " + path + name + " :\n\t") + e.what());
	}
	return (nullptr);
}
