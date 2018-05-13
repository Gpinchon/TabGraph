/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/01 20:26:08 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "parser/GLSL.hpp"

Material::Material(const std::string &name) : shader(nullptr),
	albedo(new_vec3(0.5, 0.5, 0.5)), uv_scale(new_vec2(1, 1)), alpha(1), texture_albedo(nullptr)
{
	set_name(name);
	if (!(shader = Shader::get_by_name("default")))
		shader = GLSL::parse("default", "./res/shaders/default.vert", "./res/shaders/default.frag");
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
	Material	*mtl;

	mtl = new Material(name);
	mtl->shader = Shader::get_by_name("default");
	Engine::add(*mtl);
	return (mtl);
}

Material	*Material::get_by_name(const std::string &name)
{
	int			i;
	size_t			h;
	Material	*m;

	i = 0;
	std::hash<std::string> hash_fn;
	h = hash_fn(name);
	while ((m = Engine::material(i)))
	{
		if (h == m->_id)
			return (m);
		i++;
	}
	return (nullptr);
}

void	Material::bind_textures()
{
	shader->bind_texture("in_Texture_Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("in_Use_Texture_Albedo", texture_albedo ? true : false);
}

void	Material::bind_values()
{
	shader->set_uniform("in_Albedo", albedo);
	shader->set_uniform("in_UVScale", uv_scale);
	shader->set_uniform("in_Alpha", alpha);
}

void	Material::load_textures()
{
	if (texture_albedo)
		texture_albedo->load();
}
