/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Material.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 20:40:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/10 12:52:18 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Window.hpp"
#include "Cubemap.hpp"
#include "Material.hpp"
#include "parser/BMP.hpp"
#include "parser/GLSL.hpp"

Texture	*Material::_texture_brdf = nullptr;

Material::Material(const std::string &name)
{
	set_name(name);
	if ((shader = Shader::get_by_name("default")) == nullptr) {
		shader = GLSL::parse("default", Engine::program_path() + "./res/shaders/default.vert", Engine::program_path() + "./res/shaders/default.frag");
	}
	if (_texture_brdf == nullptr)
	{
		_texture_brdf = BMP::parse("brdf", Engine::program_path() + "./res/brdfLUT.bmp");
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		_texture_brdf->set_parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
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
		if (h == m->id()) {
			return (m);
		}
		i++;
	}
	return (nullptr);
}

void	Material::bind_textures()
{
	shader->bind_texture("Material.Texture.Albedo", texture_albedo, GL_TEXTURE1);
	shader->set_uniform("Material.Texture.Use_Albedo", texture_albedo != nullptr);
	shader->bind_texture("Material.Texture.Specular", texture_specular, GL_TEXTURE2);
	shader->set_uniform("Material.Texture.Use_Specular", texture_specular != nullptr ? true : false);
	shader->bind_texture("Material.Texture.Roughness", texture_roughness, GL_TEXTURE3);
	shader->set_uniform("Material.Texture.Use_Roughness", texture_roughness != nullptr ? true : false);
	shader->bind_texture("Material.Texture.Metallic", texture_metallic, GL_TEXTURE4);
	shader->set_uniform("Material.Texture.Use_Metallic", texture_metallic != nullptr ? true : false);
	shader->bind_texture("Material.Texture.Emitting", texture_emitting, GL_TEXTURE5);
	shader->set_uniform("Material.Texture.Use_Emitting", texture_emitting != nullptr ? true : false);
	shader->bind_texture("Material.Texture.Normal", texture_normal, GL_TEXTURE6);
	shader->set_uniform("Material.Texture.Use_Normal", texture_normal != nullptr ? true : false);
	shader->bind_texture("Material.Texture.Height", texture_height, GL_TEXTURE7);
	shader->set_uniform("Material.Texture.Use_Height", texture_height != nullptr ? true : false);
	shader->bind_texture("Material.Texture.AO", texture_ao, GL_TEXTURE8);
	shader->bind_texture("Material.Texture.BRDF", _texture_brdf, GL_TEXTURE9);
	shader->bind_texture("Environment.Diffuse", Engine::current_environment()->diffuse, GL_TEXTURE11);
	shader->bind_texture("Environment.Irradiance", Engine::current_environment()->brdf, GL_TEXTURE12);
}

void	Material::bind_values()
{
	auto	res = Window::internal_resolution();
	shader->set_uniform("in_Resolution", new_vec3(res.x, res.y, res.x / res.y));
	shader->set_uniform("in_Time", SDL_GetTicks() / 1000.f);
	shader->set_uniform("Material.Albedo", albedo);
	shader->set_uniform("Material.Specular", specular);
	shader->set_uniform("Material.Emitting", emitting);
	shader->set_uniform("Material.Roughness", roughness);
	shader->set_uniform("Material.Metallic", metallic);
	shader->set_uniform("Material.Parallax", parallax);
	shader->set_uniform("Material.Alpha", alpha);
	shader->set_uniform("Material.Texture.Scale", uv_scale);
}
