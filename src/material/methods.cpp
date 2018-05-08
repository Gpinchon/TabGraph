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

Material::Material(const std::string &name) : shader(nullptr)
{
	set_name(name);
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
	ULL			h;
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

Texture	*PBRMaterial::_texture_brdf = nullptr;

PBRMaterial::PBRMaterial(const std::string &name) : Material(name),
	texture_albedo(nullptr),
	texture_specular(nullptr),
	texture_roughness(nullptr),
	texture_metallic(nullptr),
	texture_emitting(nullptr),
	texture_normal(nullptr),
	texture_height(nullptr),
	texture_ao(nullptr)
{
	//std::cout << "PBRMaterial::PBRMaterial " << name << std::endl;
	if (!_texture_brdf)
	{
		_texture_brdf = BMP::parse("brdf", "./res/brdfLUT.bmp");
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if (!(shader = Shader::get_by_name("defaultPBR")))
		shader = Shader::load("defaultPBR", "./res/shaders/defaultPBR.vert", "./res/shaders/defaultPBR.frag");
	//std::cout << shader->name();
}

PBRMaterial	*PBRMaterial::create(const std::string &name)
{
	PBRMaterial	*mtl = new PBRMaterial(name);

	mtl->alpha = 1;
	mtl->parallax = 0.01;
	mtl->albedo = new_vec3(0.5, 0.5, 0.5);
	mtl->emitting = new_vec3(0, 0, 0);
	mtl->metallic = 0;
	mtl->roughness = 0.5;
	mtl->specular = new_vec3(0.04, 0.04, 0.04);
	
	Engine::add(*mtl);
	return (mtl);
}

void	PBRMaterial::bind_textures()
{
	//std::cout << "PBRMaterial::bind_textures\n";
	shader->bind_texture("in_Texture_Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("in_Use_Texture_Albedo", texture_albedo ? true : false);
	shader->bind_texture("in_Texture_Specular", texture_specular, GL_TEXTURE2);
	shader->set_uniform("in_Use_Texture_Specular", texture_specular ? true : false);
	shader->bind_texture("in_Texture_Roughness", texture_roughness, GL_TEXTURE3);
	shader->set_uniform("in_Use_Texture_Roughness", texture_roughness ? true : false);
	shader->bind_texture("in_Texture_Metallic", texture_metallic, GL_TEXTURE4);
	shader->set_uniform("in_Use_Texture_Metallic", texture_metallic ? true : false);
	shader->bind_texture("in_Texture_Emitting", texture_emitting, GL_TEXTURE5);
	shader->set_uniform("in_Use_Texture_Emitting", texture_emitting ? true : false);
	shader->bind_texture("in_Texture_Normal", texture_normal, GL_TEXTURE6);
	shader->set_uniform("in_Use_Texture_Normal", texture_normal ? true : false);
	shader->bind_texture("in_Texture_Height", texture_height, GL_TEXTURE7);
	shader->set_uniform("in_Use_Texture_Height", texture_height ? true : false);
	shader->bind_texture("in_Texture_AO", texture_ao, GL_TEXTURE8);
	shader->set_uniform("in_Use_Texture_AO", texture_ao ? true : false);
		shader->bind_texture("in_Texture_Env",
		Engine::current_environment()->diffuse, GL_TEXTURE11);
	shader->bind_texture("in_Texture_Env_Spec",
		Engine::current_environment()->brdf, GL_TEXTURE12);
	shader->bind_texture("in_Texture_BRDF", _texture_brdf, GL_TEXTURE13);
}

void	PBRMaterial::bind_values()
{
	//std::cout << "PBRMaterial::bind_values\n";
	shader->set_uniform("in_Albedo", albedo);
	shader->set_uniform("in_Specular", specular);
	shader->set_uniform("in_Emitting", emitting);
	shader->set_uniform("in_Roughness", roughness);
	shader->set_uniform("in_Metallic", metallic);
	shader->set_uniform("in_Alpha", alpha);
	shader->set_uniform("in_Parallax", parallax);
}

void				PBRMaterial::load_textures()
{
	_texture_brdf->load();
	if (texture_albedo)
		texture_albedo->load();
	if (texture_specular)
		texture_specular->load();
	if (texture_roughness)
		texture_roughness->load();
	if (texture_metallic)
		texture_metallic->load();
	if (texture_emitting)
		texture_emitting->load();
	if (texture_normal)
		texture_normal->load();
	if (texture_height)
		texture_height->load();
	if (texture_ao)
		texture_ao->load();
}
