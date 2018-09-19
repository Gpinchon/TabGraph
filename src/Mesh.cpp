/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/19 22:59:11 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "AABB.hpp"
#include "Vgroup.hpp"
#include <algorithm>


Mesh::Mesh(const std::string &name) : RenderableMultiDraw(name)
{
	bounding_element = new AABB;
}

Mesh	*Mesh::get_by_name(const std::string &name)
{
	return dynamic_cast<Mesh *>(Renderable::get_by_name(name));
}

Mesh	*Mesh::create(const std::string &name)
{
	auto *m = new Mesh(name);
	Engine::add(*m);
	return (m);
}

void	Mesh::add(Vgroup *group)
{
	if (nullptr == group)
		return ;
	_vgroups.push_back(group);
	materials.insert(group->material);
}

void	Mesh::load()
{
	if (_is_loaded)
		return ;
	for (auto vg : _vgroups) {
		vg->load();
	}
}

#include <iostream>

bool	Mesh::render_depth(RenderMod mod)
{
	bool	ret = false;
	auto	mvp = mat4_combine(Engine::current_camera()->projection(), Engine::current_camera()->view(), transform());
	auto	normal_matrix = mat4_transpose(mat4_inverse(transform()));
	
	load();
	Shader	*last_shader = nullptr;
	for (auto vg : _vgroups) {
		if (vg->material == nullptr)
			continue ;
		vg->material->depth_shader->use();
		if (last_shader != vg->material->depth_shader) {
			vg->material->depth_shader->set_uniform("Matrix.Model", transform());
			vg->material->depth_shader->set_uniform("Matrix.ModelViewProjection", mvp);
			vg->material->depth_shader->set_uniform("Matrix.Normal", normal_matrix);
		}
		//vg->material->depth_shader->use(false);
		if (vg->render_depth(mod))
			ret = true;
		last_shader = vg->material->depth_shader;
	}
	return (ret);
}

bool	Mesh::render(RenderMod mod)
{
	bool	ret = false;
	auto	mvp = mat4_combine(Engine::current_camera()->projection(), Engine::current_camera()->view(), transform());
	auto	normal_matrix = mat4_transpose(mat4_inverse(transform()));

	load();
	Shader	*last_shader = nullptr;
	for (auto vg : _vgroups) {
		if (vg->material == nullptr)
			continue ;
		vg->material->shader->use();
		if (last_shader != vg->material->shader) {
			vg->material->shader->set_uniform("Matrix.Model", transform());
			vg->material->shader->set_uniform("Matrix.ModelViewProjection", mvp);
			vg->material->shader->set_uniform("Matrix.Normal", normal_matrix);
		}
		//vg->material->shader->use(false);
		if (vg->render(mod))
			ret = true;
		last_shader = vg->material->shader;
	}
	return (ret);
}

void	Mesh::set_cull_mod(GLenum mod)
{
	_cull_mod = mod;
}

/*void	Mesh::sort(renderable_compare compare)
{
	std::sort(_vgroups.begin(), _vgroups.end(), compare);
}*/

void	Mesh::center()
{
	for (auto vg : _vgroups) {
		vg->center(bounding_element->center);
	}
	bounding_element->min = vec3_sub(bounding_element->min, bounding_element->center);
	bounding_element->max = vec3_sub(bounding_element->max, bounding_element->center);
	bounding_element->center = new_vec3(0, 0, 0);
}
