/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vgroup.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:48:07 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:07:56 by gpinchon         ###   ########.fr       */
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

std::vector<std::shared_ptr<Vgroup>> Vgroup::_vgroups;

Vgroup::Vgroup(const std::string &name) : Renderable(name)
{
	bounding_element = new AABB;
}

std::shared_ptr<Vgroup>	Vgroup::create(const std::string &name)
{
	auto	vg = std::shared_ptr<Vgroup>(new Vgroup(name));
	_vgroups.push_back(vg);
	return (vg);
}

std::shared_ptr<Vgroup>	Vgroup::get(unsigned index)
{
	if (index >= _vgroups.size())
		return (nullptr);
	return (_vgroups.at(index));
}

std::shared_ptr<Vgroup>	Vgroup::get_by_name(const std::string &name)
{
	std::hash<std::string>	hash_fn;
	auto					h = hash_fn(name);
	for (auto n : _vgroups) {
		if (h == n->id())
			return (n);
	}
	return (nullptr);
}

void			Vgroup::load()
{
	if (is_loaded()) {
		return ;
	}
	_vao = VertexArray::create(v.size());
	_vao->add_buffer(GL_FLOAT, 3, v);
	_vao->add_buffer(GL_UNSIGNED_BYTE, 4, vn);
	_vao->add_buffer(GL_FLOAT, 2, vt);
	_vao->add_indices(i);
	_is_loaded = true;
}

void			Vgroup::bind()
{
	if ((material() == nullptr) || (material()->shader() == nullptr) || (Camera::current() == nullptr)) {
		return ;
	}
}

bool			Vgroup::render_depth(RenderMod mod)
{
	auto	mtl = material();
	if ((mtl == nullptr)) {
		return (false);
	}
	auto	depthShader = mtl->depth_shader();
	if (depthShader == nullptr) {
		return (false);
	}
	if (mod == RenderOpaque
		&& (mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4)))
		return (false);
	else if (mod == RenderTransparent
		&&	!(mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4))) {
		return (false);
	}
	depthShader->use();
	depthShader->bind_texture("Texture.Albedo", mtl->texture_albedo(), GL_TEXTURE0);
	depthShader->set_uniform("Texture.Use_Albedo", mtl->texture_albedo() != nullptr);
	depthShader->set_uniform("Material.Alpha", mtl->alpha);
	/*material->bind_textures();
	material->bind_values();*/
	//bind();
	_vao->draw();
	depthShader->use(false);
	return (true);
}

bool			Vgroup::render(RenderMod mod)
{
	auto	mtl = material();
	if ((mtl == nullptr)) {
		return (false);
	}
	auto	cShader = mtl->shader();
	if (cShader == nullptr) {
		return (false);
	}
	if (mod == RenderOpaque
		&& (mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4)))
		return (false);
	else if (mod == RenderTransparent
		&&	!(mtl->alpha < 1 || (mtl->texture_albedo() != nullptr && mtl->texture_albedo()->values_per_pixel() == 4))) {
		return (false);
	}
	cShader->use();
	mtl->bind_textures();
	mtl->bind_values();
	bind();
	_vao->draw();
	cShader->use(false);
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
