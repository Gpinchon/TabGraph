/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/10/11 18:48:48 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Environment.hpp"
#include "Window.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "parser/BMP.hpp"
#include "parser/GLSL.hpp"

std::vector<std::shared_ptr<Material>>	Material::_materials;

Material::Material(const std::string &name) : Object(name)
{
	if ((_shader = Shader::get_by_name("shader_default")).lock() == nullptr) {
		_shader = GLSL::parse("shader_default", Engine::program_path() + "./res/shaders/forward_default.frag", ForwardShader);
	}
	if ((_depth_shader = Shader::get_by_name("default_depth")).lock() == nullptr) {
		_depth_shader = GLSL::parse("default_depth", Engine::program_path() + "./res/shaders/depth.vert", Engine::program_path() + "./res/shaders/depth.frag");
	}
}

std::shared_ptr<Material>	Material::create(const std::string &name)
{
	auto	mtl = std::shared_ptr<Material>(new Material(name));
	_materials.push_back(mtl);
	return (mtl);
}

std::shared_ptr<Material>	Material::get(unsigned index)
{
	if (index >= _materials.size())
		return (nullptr);
	return (_materials.at(index));
}

std::shared_ptr<Material>	Material::get_by_name(const std::string &name)
{
	size_t		h;
	
	std::hash<std::string> hash_fn;
	h = hash_fn(name);
	for (auto m : _materials) {
		if (h == m->id()) {
			return (m);
		}
	}
	return (nullptr);
}

void	Material::bind_textures()
{
	auto	shaderPtr = shader();
	shaderPtr->bind_texture("Texture.Albedo", texture_albedo(), GL_TEXTURE1);
	shaderPtr->set_uniform("Texture.Use_Albedo", texture_albedo() != nullptr);
	shaderPtr->bind_texture("Texture.Specular", texture_specular(), GL_TEXTURE2);
	shaderPtr->set_uniform("Texture.Use_Specular", texture_specular() != nullptr ? true : false);
	shaderPtr->bind_texture("Texture.Roughness", texture_roughness(), GL_TEXTURE3);
	shaderPtr->set_uniform("Texture.Use_Roughness", texture_roughness() != nullptr ? true : false);
	shaderPtr->bind_texture("Texture.Metallic", texture_metallic(), GL_TEXTURE4);
	shaderPtr->set_uniform("Texture.Use_Metallic", texture_metallic() != nullptr ? true : false);
	shaderPtr->bind_texture("Texture.Emitting", texture_emitting(), GL_TEXTURE5);
	shaderPtr->set_uniform("Texture.Use_Emitting", texture_emitting() != nullptr ? true : false);
	shaderPtr->bind_texture("Texture.Normal", texture_normal(), GL_TEXTURE6);
	shaderPtr->set_uniform("Texture.Use_Normal", texture_normal() != nullptr ? true : false);
	shaderPtr->bind_texture("Texture.Height", texture_height(), GL_TEXTURE7);
	shaderPtr->set_uniform("Texture.Use_Height", texture_height() != nullptr ? true : false);
	shaderPtr->bind_texture("Texture.AO", texture_ao(), GL_TEXTURE8);
	if (Environment::current() != nullptr) {
		shaderPtr->bind_texture("Environment.Diffuse", Environment::current()->diffuse(), GL_TEXTURE9);
		shaderPtr->bind_texture("Environment.Irradiance", Environment::current()->irradiance(), GL_TEXTURE10);
	}
}

void	Material::bind_values()
{
	auto	shaderPtr = shader();
	shaderPtr->set_uniform("Material.Albedo", albedo);
	shaderPtr->set_uniform("Material.Specular", specular);
	shaderPtr->set_uniform("Material.Emitting", emitting);
	shaderPtr->set_uniform("Material.Roughness", roughness);
	shaderPtr->set_uniform("Material.Metallic", metallic);
	shaderPtr->set_uniform("Material.Alpha", alpha);
	shaderPtr->set_uniform("Material.Parallax", parallax);
	shaderPtr->set_uniform("Material.Ior", ior);
	shaderPtr->set_uniform("Texture.Scale", uv_scale);
}

std::shared_ptr<Shader>		Material::shader()
{
	return (_shader.lock());
}

std::shared_ptr<Shader>		Material::depth_shader()
{
	return (_depth_shader.lock());
}

std::shared_ptr<Texture>	Material::texture_albedo()
{
	return (_texture_albedo.lock());
}

std::shared_ptr<Texture>	Material::texture_specular()
{
	return (_texture_specular.lock());
}

std::shared_ptr<Texture>	Material::texture_emitting()
{
	return (_texture_emitting.lock());
}

std::shared_ptr<Texture>	Material::texture_normal()
{
	return (_texture_normal.lock());
}

std::shared_ptr<Texture>	Material::texture_height()
{
	return (_texture_height.lock());
}

std::shared_ptr<Texture>	Material::texture_roughness()
{
	return (_texture_roughness.lock());
}

std::shared_ptr<Texture>	Material::texture_metallic()
{
	return (_texture_metallic.lock());
}

std::shared_ptr<Texture>	Material::texture_ao()
{
	return (_texture_ao.lock());
}

void						Material::set_texture_albedo(std::shared_ptr<Texture> t)
{
	_texture_albedo = t;
}

void						Material::set_texture_specular(std::shared_ptr<Texture> t)
{
	_texture_specular = t;
}

void						Material::set_texture_emitting(std::shared_ptr<Texture> t)
{
	_texture_emitting = t;
}

void						Material::set_texture_normal(std::shared_ptr<Texture> t)
{
	_texture_normal = t;
}

void						Material::set_texture_height(std::shared_ptr<Texture> t)
{
	_texture_height = t;
}

void						Material::set_texture_roughness(std::shared_ptr<Texture> t)
{
	_texture_roughness = t;
}

void						Material::set_texture_metallic(std::shared_ptr<Texture> t)
{
	_texture_metallic = t;
}

void						Material::set_texture_ao(std::shared_ptr<Texture> t)
{
	_texture_ao = t;
}
