/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/30 23:16:49 by gpinchon         ###   ########.fr       */
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

Mesh::Mesh(const std::string &name) : Renderable(name)
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

void	Mesh::load()
{
	if (_is_loaded)
		return ;
	for (auto vg : vgroups) {
		vg->load();
	}
}

void	Mesh::bind()
{
	MAT4		mvp;
	MAT4		normal_matrix;

	mvp = mat4_combine(Engine::current_camera()->projection, Engine::current_camera()->view, mat4_transform());
	normal_matrix = mat4_transpose(mat4_inverse(mat4_transform()));
	for (auto vg : vgroups) {
		vg->material->shader->use();
		vg->material->shader->set_uniform("in_Transform", mvp);
		vg->material->shader->set_uniform("in_ModelMatrix", mat4_transform());
		vg->material->shader->set_uniform("in_NormalMatrix", normal_matrix);
		vg->material->shader->use(false);
	}
}

bool	Mesh::render(RenderMod mod)
{
	bool	ret = false;
	load();
	bind();
	if (mod == RenderTransparent) {
		glDisable(GL_CULL_FACE);
		//glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE_ARB);
		//glEnable(GL_BLEND);
		//glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}
	else {
		//glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glCullFace(_cull_mod);
	}
	for (auto vg : vgroups) {
		if (vg->render(mod))
			ret = true;
	}
	return (ret);
}

void	Mesh::set_cull_mod(GLenum mod)
{
	_cull_mod = mod;
}

void	Mesh::sort(renderable_compare compare)
{
	std::sort(vgroups.begin(), vgroups.end(), compare);
}

void	Mesh::center()
{
	for (auto vg : vgroups) {
		vg->center(bounding_element->center);
	}
	bounding_element->min = vec3_sub(bounding_element->min, bounding_element->center);
	bounding_element->max = vec3_sub(bounding_element->max, bounding_element->center);
	bounding_element->center = new_vec3(0, 0, 0);
}
