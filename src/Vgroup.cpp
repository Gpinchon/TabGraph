/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vgroup.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:48:07 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/06 21:32:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Vgroup.hpp"
#include "VertexArray.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "AABB.hpp"
#include "Texture.hpp"
#include "parser/GLSL.hpp"

Vgroup::Vgroup(const std::string &name) : Renderable(name)
{
	bounding_element = new AABB;
}

Vgroup		*Vgroup::create(const std::string &name)
{
	return (new Vgroup(name));
};

void			Vgroup::load()
{
	if (is_loaded()) {
		return ;
	}
	_vao = VertexArray::create(v.size());
	_vao->add_buffer(GL_FLOAT, 3, v);
	_vao->add_buffer(GL_UNSIGNED_BYTE, 4, vn);
	_vao->add_buffer(GL_FLOAT, 2, vt);
	_is_loaded = true;
}

void			Vgroup::bind()
{
	if ((material == nullptr) || (material->shader == nullptr) || (Engine::current_camera() == nullptr)) {
		return ;
	}
	material->shader->set_uniform("in_UVMax", uvmax);
	material->shader->set_uniform("in_UVMin", uvmin);
	material->shader->set_uniform("in_CamPos", Engine::current_camera()->position());
}

bool			Vgroup::render_depth(RenderMod mod)
{
	if ((material == nullptr) || (material->depth_shader == nullptr)) {
		return (false);
	}
	if (mod == RenderOpaque
		&& (material->alpha < 1 || (material->texture_albedo != nullptr && material->texture_albedo->values_per_pixel() == 4)))
		return (false);
	else if (mod == RenderTransparent
		&&	!(material->alpha < 1 || (material->texture_albedo != nullptr && material->texture_albedo->values_per_pixel() == 4))) {
		return (false);
	}
	material->depth_shader->use();
	material->depth_shader->bind_texture("Texture.Albedo", material->texture_albedo, GL_TEXTURE0);
	material->depth_shader->set_uniform("Texture.Use_Albedo", material->texture_albedo != nullptr);
	material->depth_shader->set_uniform("Material.Alpha", material->alpha);
	/*material->bind_textures();
	material->bind_values();*/
	//bind();
	_vao->draw();
	material->depth_shader->use(false);
	return (true);
}

bool			Vgroup::render(RenderMod mod)
{
	if ((material == nullptr) || (material->shader == nullptr)) {
		return (false);
	}
	if (mod == RenderOpaque
		&& (material->alpha < 1 || (material->texture_albedo != nullptr && material->texture_albedo->values_per_pixel() == 4)))
		return (false);
	else if (mod == RenderTransparent
		&&	!(material->alpha < 1 || (material->texture_albedo != nullptr && material->texture_albedo->values_per_pixel() == 4))) {
		return (false);
	}
	material->shader->use();
	material->bind_textures();
	material->bind_values();
	bind();
	_vao->draw();
	material->shader->use(false);
	return (true);
}

void			Vgroup::center(VEC3 &center)
{
	for (auto &vec : v) {
		vec = vec3_sub(vec, center);
	}
	bounding_element->min = vec3_sub(bounding_element->min, center);
	bounding_element->max = vec3_sub(bounding_element->max, center);
	bounding_element->center = vec3_sub(bounding_element->center, center);
	position() = vec3_sub(bounding_element->center, center);
}
