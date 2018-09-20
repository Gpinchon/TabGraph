/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cubemap.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 16:36:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/20 19:25:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Cubemap.hpp"
#include "Errors.hpp"
#include "parser/BMP.hpp"
#include <thread>

void	cubemap_load_side(std::shared_ptr<Cubemap> cubemap, const std::string &path, GLenum iside)
{
	auto	sideTexture = BMP::parse(path, path);
	if (sideTexture == nullptr){
		return ;
	}
	cubemap->set_side(iside - GL_TEXTURE_CUBE_MAP_POSITIVE_X, sideTexture);
}

void	Cubemap::load()
{
	if (_loaded) {
		return ;
	}
	Texture::load();
	for (auto i = 0u; i < 6; i++) {
		auto t = side(i);
		t->load();
		assign(*t, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
	}
	generate_mipmap();
}

void	Cubemap::unload()
{
	if (!_loaded) {
		return ;
	}
	for (auto i = 0; i < 6; i++)
		side(i)->unload();
	Texture::unload();
}


Cubemap::Cubemap(const std::string &name) : Texture(name)
{
	_target = GL_TEXTURE_CUBE_MAP;
}

std::shared_ptr<Cubemap>	Cubemap::create(const std::string &name)
{
	auto	cubemap = std::shared_ptr<Cubemap>(new Cubemap(name));
	glGenTextures(1, &cubemap->_glid);
	glBindTexture(cubemap->_target, cubemap->_glid);
	glBindTexture(cubemap->_target, 0);
	_textures.push_back(std::static_pointer_cast<Texture>(cubemap));
	_cubemaps.push_back(cubemap);
#ifdef GL_DEBUG
	glObjectLabel(GL_TEXTURE, cubemap->_glid, -1, cubemap->name().c_str());
	glCheckError();
#endif //GL_DEBUG 
	return (cubemap);
}

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

#include <iostream>


void	generate_side(std::shared_ptr<Texture> fromTexture, std::shared_ptr<Texture> t, int side)
{
	const float ftu = faceTransform[side][1];
	const float ftv = faceTransform[side][0];
	static const float an = sin(M_PI / 4.f);
	static const float ak = cos(M_PI / 4.f);
	
	for (auto x = 0; x <= t->size().x; ++x)
	{
		for (auto y = 0; y <= t->size().y; ++y)
		{
			float nx = (float)y / (float)t->size().x - 0.5f;
			float ny = 1 - (float)x / (float)t->size().y - 0.5f;
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
			auto	nuv = vec2_div(new_vec2(y, x), t->size());
			auto	val = fromTexture->sample(new_vec2(u, v));
			t->set_pixel(new_vec2(nuv.x, nuv.y), val);
		}
	}
	std::cout << "." << std::flush;
}

std::shared_ptr<Cubemap>	Cubemap::create(const std::string &name, std::shared_ptr<Texture> fromTexture)
{
	std::cout << "Converting " << fromTexture->name() << " into Cubemap";
	auto	cubemap = Cubemap::create(name);
	GLenum formats[2];
	fromTexture->format(&formats[0], &formats[1]);
	std::vector<std::thread> threads;
	for (auto i = 0; i < 6; ++i)
	{
		auto	side_res = fromTexture->size().x / 4.f;
		auto	sideTexture = Texture::create(fromTexture->name() + "_side_" + std::to_string(i), new_vec2(side_res, side_res), GL_TEXTURE_2D, formats[0], formats[1], fromTexture->data_format());
		threads.push_back(std::thread(generate_side, fromTexture, sideTexture, i));
		cubemap->set_side(i, sideTexture);
	}
	for (auto i = 0u; i < threads.size(); i++)
		threads.at(i).join();
	cubemap->generate_mipmap();
	cubemap->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	std::cout << " Done." << std::endl;
	return (cubemap);
}

std::shared_ptr<Cubemap>	Cubemap::parse(const std::string &name, const std::string &path)
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
		t->set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		return (t);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing Cubemap : " + path + name + " :\n\t") + e.what());
	}
	return (nullptr);
}
