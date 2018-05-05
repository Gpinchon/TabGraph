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
	std::cout << "Material::get_by_name " << name << std::endl;
	while ((m = Engine::material(i)))
	{
		std::cout << m->name() << " " << m->_id << std::endl;
		if (h == m->_id)
			return (m);
		i++;
	}
	return (nullptr);
}

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
	mtl->shader = Shader::get_by_name("defaultPBR");
	Engine::add(*mtl);
	return (mtl);
}

void	PBRMaterial::bind_textures()
{
	shader->bind_texture("in_Texture_Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("in_Use_Texture_Albedo", texture_albedo ? false : true);
	shader->bind_texture("in_Texture_Specular", texture_specular, GL_TEXTURE2);
	shader->set_uniform("in_Use_Texture_Specular", texture_specular ? false : true);
	shader->bind_texture("in_Texture_Roughness", texture_roughness, GL_TEXTURE3);
	shader->set_uniform("in_Use_Texture_Roughness", texture_roughness ? false : true);
	shader->bind_texture("in_Texture_Metallic", texture_metallic, GL_TEXTURE4);
	shader->set_uniform("in_Use_Texture_Metallic", texture_metallic ? false : true);
	shader->bind_texture("in_Texture_Emitting", texture_emitting, GL_TEXTURE5);
	shader->set_uniform("in_Use_Texture_Emitting", texture_emitting ? false : true);
	shader->bind_texture("in_Texture_Normal", texture_normal, GL_TEXTURE6);
	shader->set_uniform("in_Use_Texture_Normal", texture_normal ? false : true);
	shader->bind_texture("in_Texture_Height", texture_height, GL_TEXTURE7);
	shader->set_uniform("in_Use_Texture_Height", texture_height ? false : true);
	shader->bind_texture("in_Texture_AO", texture_ao, GL_TEXTURE8);
	shader->set_uniform("in_Use_Texture_AO", texture_ao ? false : true);
	/*shader->bind_texture("in_Texture_Env",
		Engine::get().env, GL_TEXTURE11);
	shader->bind_texture("in_Texture__env_brdf",
		Engine::get()._env_brdf, GL_TEXTURE12);
	shader->bind_texture("in_Texture_BRDF",
		Engine::get().brdf_lut, GL_TEXTURE13);*/
}

void	PBRMaterial::bind_values()
{
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
	texture_albedo->load();
	texture_specular->load();
	texture_roughness->load();
	texture_metallic->load();
	texture_emitting->load();
	texture_normal->load();
	texture_height->load();
	texture_ao->load();
}
