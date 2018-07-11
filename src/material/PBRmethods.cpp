/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PBRmethods.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 18:17:13 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/08 17:24:05 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Cubemap.hpp"
#include "PBRMaterial.hpp"
#include "parser/BMP.hpp"
#include "parser/GLSL.hpp"

Texture	*PBRMaterial::_texture_brdf = nullptr;

PBRMaterial::PBRMaterial(const std::string &name) : Material(name),
	texture_roughness(nullptr),
	texture_metallic(nullptr),
	texture_ao(nullptr)
{
	if (_texture_brdf == nullptr)
	{
		_texture_brdf = BMP::parse("brdf", Engine::program_path() + "./res/brdfLUT.bmp");
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	if ((shader = Shader::get_by_name("defaultPBR")) == nullptr) {
		shader = GLSL::parse("defaultPBR",  Engine::program_path() + "./res/shaders/defaultPBR.vert",  Engine::program_path() + "./res/shaders/defaultPBR.frag");
}
}

PBRMaterial	*PBRMaterial::create(const std::string &name)
{
	auto	*mtl = new PBRMaterial(name);

	mtl->parallax = 0.01;
	mtl->emitting = new_vec3(0, 0, 0);
	mtl->metallic = 0;
	mtl->roughness = 0.5;
	mtl->specular = new_vec3(0.04, 0.04, 0.04);
	Engine::add(*mtl);
	return (mtl);
}

void	PBRMaterial::bind_textures()
{
	Material::bind_textures();
	shader->bind_texture("in_Texture_Specular", texture_specular, GL_TEXTURE2);
	shader->set_uniform("in_Use_Texture_Specular", texture_specular != nullptr ? true : false);
	shader->bind_texture("in_Texture_Roughness", texture_roughness, GL_TEXTURE3);
	shader->set_uniform("in_Use_Texture_Roughness", texture_roughness != nullptr ? true : false);
	shader->bind_texture("in_Texture_Metallic", texture_metallic, GL_TEXTURE4);
	shader->set_uniform("in_Use_Texture_Metallic", texture_metallic != nullptr ? true : false);
	shader->bind_texture("in_Texture_Emitting", texture_emitting, GL_TEXTURE5);
	shader->set_uniform("in_Use_Texture_Emitting", texture_emitting != nullptr ? true : false);
	shader->bind_texture("in_Texture_Normal", texture_normal, GL_TEXTURE6);
	shader->set_uniform("in_Use_Texture_Normal", texture_normal != nullptr ? true : false);
	shader->bind_texture("in_Texture_Height", texture_height, GL_TEXTURE7);
	shader->set_uniform("in_Use_Texture_Height", texture_height != nullptr ? true : false);
	shader->bind_texture("in_Texture_AO", texture_ao, GL_TEXTURE8);
	shader->set_uniform("in_Use_Texture_AO", texture_ao != nullptr ? true : false);
	shader->bind_texture("in_Texture_BRDF", _texture_brdf, GL_TEXTURE13);
}

void	PBRMaterial::bind_values()
{
	Material::bind_values();
	shader->set_uniform("in_Specular", specular);
	shader->set_uniform("in_Emitting", emitting);
	shader->set_uniform("in_Roughness", roughness);
	shader->set_uniform("in_Metallic", metallic);
	shader->set_uniform("in_Parallax", parallax);
}

void	PBRMaterial::load_textures()
{
/*	Material::load_textures();
	_texture_brdf->load();
	if (texture_specular != nullptr) {
		texture_specular->load();
}
	if (texture_roughness != nullptr) {
		texture_roughness->load();
}
	if (texture_metallic != nullptr) {
		texture_metallic->load();
}
	if (texture_emitting != nullptr) {
		texture_emitting->load();
}
	if (texture_normal != nullptr) {
		texture_normal->load();
}
	if (texture_height != nullptr) {
		texture_height->load();
}
	if (texture_ao != nullptr) {
		texture_ao->load();
}
*/
}