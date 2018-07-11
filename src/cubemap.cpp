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

#include <iostream>

VEC3	outImgToXYZ(float u, float v, int faceIdx)
{
	VEC3	xyz;
	auto	a = 2.0 * u;
	auto	b = 2.0 * v;

	if (faceIdx == 0) // back
		xyz = new_vec3(-1.0, 1.0 - a, 1.0 - b);
	else if (faceIdx == 1) // left
		xyz = new_vec3(a - 1.0, -1.0, 1.0 - b);
	else if (faceIdx == 2) // front)
		xyz = new_vec3(1.0, a - 1.0, 1.0 - b);
	else if (faceIdx == 3) // right)
		xyz = new_vec3(1.0 - a, 1.0, 1.0 - b);
	else if (faceIdx == 4) // top
		xyz = new_vec3(b - 1.0, a - 1.0, 1.0);
	else if (faceIdx == 5) // bottom
		xyz = new_vec3(1.0 - b, a - 1.0, -1.0);
	return (xyz);
}

float faceTransform[6][2] = 
{ 
	{0, 0},
	{0, M_PI},
	{M_PI, 0},
	{-M_PI / 2.f, 0},
	{0, -M_PI / 2.f},
	{0, M_PI / 2.f}
};

Cubemap		*Cubemap::create(const std::string &name, Texture *fromTexture)
{
	auto	cubemap = new Cubemap(name);
	GLenum formats[2];
	fromTexture->format(&formats[0], &formats[1]);
	//fromTexture->resize(new_vec2(2048, 2048));
	const float an = sin(M_PI / 4.f);
	const float ak = cos(M_PI / 4.f);
	for (auto side = 0; side < 6; ++side)
	{
		GLenum formats[2];
		fromTexture->format(&formats[0], &formats[1]);
		auto	sideTexture = Texture::create(fromTexture->name() + "side", new_vec2(512, 512), GL_TEXTURE_2D, formats[0], formats[1], fromTexture->data_format());
		const float ftu = faceTransform[side][1];
		const float ftv = faceTransform[side][0];
		for (auto y = 0; y < sideTexture->size().x; ++y)
		{
			for (auto x = 0; x < sideTexture->size().x; ++x)
			{
				float nx = (float)y / (float)sideTexture->size().x - 0.5f;
				float ny = (float)x / (float)sideTexture->size().x - 0.5f;

				nx *= 2;
				ny *= 2;
				nx *= an; 
				ny *= an; 

				float u, v;
				if(ftv == 0) {
					u = atan2(nx, ak);
					v = atan2(ny * cos(u), ak);
					u += ftu; 
				} else if(ftv > 0) { 
					float d = sqrt(nx * nx + ny * ny);
					v = M_PI / 2.f - atan2(d, ak);
					u = atan2(ny, nx);
				} else {
					float d = sqrt(nx * nx + ny * ny);
					v = -M_PI / 2.f + atan2(d, ak);
					u = atan2(-ny, nx);
				}
				u = u / (M_PI);
				v = v / (M_PI / 2.f);
				while (v < -1) {
					v += 2;
					u += 1;
				} 
				while (v > 1) {
					v -= 2;
					u += 1;
				} 

				while(u < -1) {
					u += 2;
				}
				while(u > 1) {
					u -= 2;
				}

				u = u / 2.0f + 0.5f;
				v = v / 2.0f + 0.5f;

				//u = u * (fromTexture->size().x - 1);
				//v = v * (fromTexture->size().y - 1);
				auto	nuv = vec2_div(new_vec2(y, x), sideTexture->size());
				auto	val = fromTexture->texelfetch(new_vec2(u, v));
				sideTexture->set_pixel(new_vec2(nuv.x, 1 - nuv.y), val);
				//sideTexture->set_pixel(new_vec2(nuv.x, 1 - nuv.y), val);
				//std::cout << val.x << " " << val.y << " " << val.z << std::endl;
				//std::cout << u << " " << v << std::endl;
			}
		}
		cubemap->assign(*sideTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side);
		cubemap->sides.at(side) = sideTexture;
	}
	/*for (auto side = 0; side < 6; ++side)
	{
		auto	sideTexture = Texture::create(fromTexture->name() + "side", new_vec2(fromTexture->size().x, fromTexture->size().x), GL_TEXTURE_2D, formats[0], formats[1], fromTexture->data_format());
		auto	inSize = fromTexture->size();

		for (auto xOut = 0; xOut < sideTexture->size().x; ++xOut) {
			for (auto yOut = 0; yOut < sideTexture->size().x; ++yOut) {
				auto	uv = new_vec2(
					xOut / sideTexture->size().x,
					yOut / sideTexture->size().y);
				auto	xyz = outImgToXYZ(uv.x, uv.y, side);
	            auto	theta = atan2(xyz.y, xyz.x);// # range -M_PI to M_PI
	            //auto	phi = M_PI * sqrt(xyz.x * xyz.x + xyz.y * xyz.y);
	            auto	r = hypot(xyz.x, xyz.y);
	            auto	phi = atan2(xyz.z,r);// # range -M_PI/2 to M_PI/2
	            //# source img coords
	            auto	uf = 0.5 * inSize.x * (theta + M_PI) / M_PI / inSize.x;
	            auto	vf = 0.5 * inSize.y * (M_PI/2 - phi) / M_PI / inSize.y;
	            auto	p = fromTexture->texelfetch(new_vec2(uf, vf));
	            sideTexture->set_pixel(uv, p);
	        }
	    }
	    cubemap->assign(*sideTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X + side);
	    cubemap->sides.at(side) = sideTexture;
	}*/
/*	cubemap->assign(*fromTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	cubemap->sides.at(0) = fromTexture;
	cubemap->assign(*fromTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	cubemap->sides.at(1) = fromTexture;
	cubemap->assign(*fromTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	cubemap->sides.at(2) = fromTexture;
	cubemap->assign(*fromTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	cubemap->sides.at(3) = fromTexture;
	cubemap->assign(*fromTexture, GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	cubemap->sides.at(4) = fromTexture;
	cubemap->assign(*fromTexture, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	cubemap->sides.at(5) = fromTexture;*/
	cubemap->generate_mipmap();
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