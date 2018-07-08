/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/08 21:13:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Window.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "parser/GLSL.hpp"

Material::Material(const std::string &name) : shader(nullptr),
	albedo(new_vec3(0.5, 0.5, 0.5)), uv_scale(new_vec2(1, 1)), alpha(1),
	texture_albedo(nullptr),
	texture_specular(nullptr),
	texture_emitting(nullptr),
	texture_normal(nullptr),
	texture_height(nullptr)
{
	set_name(name);
	if ((shader = Shader::get_by_name("default")) == nullptr) {
		shader = GLSL::parse("default", Engine::program_path() + "./res/shaders/default.vert", Engine::program_path() + "./res/shaders/default.frag");
	}
}

void	Material::set_name(const std::string &name)
{
	_name = name;
	std::hash<std::string> hash_fn;
	_id = hash_fn(name);
}

const std::string	&Material::name()
{
	return (_name);
}

Material	*Material::create(const std::string &name)
{
	auto	mtl = new Material(name);
	mtl->shader = Shader::get_by_name("default");
	Engine::add(*mtl);
	return (mtl);
}

Material	*Material::get_by_name(const std::string &name)
{
	auto		i = 0;
	size_t		h;
	Material	*m;
	
	std::hash<std::string> hash_fn;
	h = hash_fn(name);
	while ((m = Engine::material(i)) != nullptr)
	{
		if (h == m->_id) {
			return (m);
		}
		i++;
	}
	return (nullptr);
}

void	Material::bind_textures()
{
	shader->bind_texture("in_Texture_Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("in_Use_Texture_Albedo", texture_albedo != nullptr);
	shader->bind_texture("in_Texture_Env", Engine::current_environment()->diffuse, GL_TEXTURE11);
	shader->bind_texture("in_Texture_Env_Spec", Engine::current_environment()->brdf, GL_TEXTURE12);

}

void	Material::bind_values()
{
	shader->set_uniform("in_Resolution", Window::internal_resolution());
	shader->set_uniform("in_Time", SDL_GetTicks() / 1000.f);
	shader->set_uniform("in_Albedo", albedo);
	shader->set_uniform("in_UVScale", uv_scale);
	shader->set_uniform("in_Alpha", alpha);
}

void	Material::load_textures()
{
	/*if (texture_albedo != nullptr) {
		texture_albedo->load();
	}*/
}
