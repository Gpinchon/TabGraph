/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/01 15:43:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <algorithm>

Mesh::Mesh(const std::string &name) : Renderable(name)
{
	bounding_element = new AABB;
}

Renderable	*Renderable::get_by_name(const std::string &name)
{
	return dynamic_cast<Renderable *>(Node::get_by_name(name));
}

Mesh	*Mesh::get_by_name(const std::string &name)
{
	return dynamic_cast<Mesh *>(Renderable::get_by_name(name));
}

bool	alpha_compare(Renderable	*m, Renderable *m1)
{
	auto	mat = m->material;
	auto	mat1 = m1->material;
	if (m->parent == m1) {
		return (false);
	} if (m1->parent == m) {
		return (true);
	}
	if ((mat == nullptr) && (mat1 == nullptr)) {
		return (false);
	}
	if (mat == nullptr) {
		return (true);
	}
	if (mat1 == nullptr) {
		return (false);
	}
	return mat->alpha > mat1->alpha || ((mat->texture_albedo != nullptr) && (mat1->texture_albedo != nullptr) &&
		mat->texture_albedo->bpp() <= 24 && mat1->texture_albedo->bpp() >= 32);
}

void	Renderable::alpha_sort()
{
	Engine::sort(alpha_compare);
}

Mesh	*Mesh::create(const std::string &name)
{
	auto *m = new Mesh(name);
	Engine::add(*m);
	return (m);
}

Vgroup::Vgroup(const std::string &name) : Renderable(name)
{
	bounding_element = new AABB;
}

Vgroup		*Vgroup::create(const std::string &name)
{
	return (new Vgroup(name));
};

template <typename T>
static GLuint	vbuffer_load(GLuint attrib, int size, const std::vector<T> &a)
{
	GLuint	lbufferid;

	if (!size || a.empty()) {
		return (-1);
}
	glGenBuffers(1, &lbufferid);
	glBindBuffer(GL_ARRAY_BUFFER, lbufferid);
	glBufferData(GL_ARRAY_BUFFER, a.size() * sizeof(T), &a[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, size,
		sizeof(T) == 4 ? GL_UNSIGNED_BYTE : GL_FLOAT,
		GL_FALSE, 0, (void*)nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return (lbufferid);
}

void			Vgroup::load()
{
	if (is_loaded()) {
		return ;
	}
	if (glIsVertexArray(v_arrayid)) {
		glDeleteVertexArrays(1, &v_arrayid);
	}
	glGenVertexArrays(1, &v_arrayid);
	glBindVertexArray(v_arrayid);
	if (glIsBuffer(v_bufferid)) {
		glDeleteBuffers(1, &v_bufferid);
	}
	v_bufferid = vbuffer_load(0, 3, v);
	if (glIsBuffer(vn_bufferid)) {
		glDeleteBuffers(1, &v_bufferid);
	}
	vn_bufferid = vbuffer_load(1, 4, vn);
	if (glIsBuffer(vt_bufferid)) {
		glDeleteBuffers(1, &v_bufferid);
	}
	vt_bufferid = vbuffer_load(2, 2, vt);
	glBindVertexArray(0);
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

void			Vgroup::render()
{
	if ((material == nullptr) || (material->shader == nullptr)) {
		return ;
	}
	material->shader->use();
	material->bind_textures();
	material->bind_values();
	bind();
	if (_cull_mod == 0) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
		glCullFace(_cull_mod);
	}
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(v_arrayid);
	glDrawArrays(GL_TRIANGLES, 0, v.size());
	glBindVertexArray(0);
	material->shader->use(false);
}

void			Mesh::load()
{
	if (_is_loaded)
		return ;
	for (auto vg : vgroups) {
		vg->load();
	}
}

void			Mesh::bind()
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

void			Mesh::render()
{
	load();
	bind();
	for (auto vg : vgroups) {
		vg->render();
	}
}

void		Mesh::sort(renderable_compare compare)
{
	std::sort(vgroups.begin(), vgroups.end(), compare);
}

void			Mesh::center()
{
	for (auto vg : vgroups) {
		vg->center(bounding_element->center);
	}
	bounding_element->min = vec3_sub(bounding_element->min, bounding_element->center);
	bounding_element->max = vec3_sub(bounding_element->max, bounding_element->center);
	bounding_element->center = new_vec3(0, 0, 0);
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
