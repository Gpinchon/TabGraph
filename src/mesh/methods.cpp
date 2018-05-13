/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/09 23:58:04 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "Material.hpp"

Mesh::Mesh(const std::string &name) : Renderable(name),
uvmax(new_vec2(1, 1)), uvmin(new_vec2(0, 0)),
v_arrayid(0), v_bufferid(0), vn_bufferid(0), vt_bufferid(0)
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
	if (m->parent == m1)
		return (false);
	else if (m1->parent == m)
		return (true);
	if (!mat && !mat1)
		return (false);
	if (!mat)
		return (true);
	if (!mat1)
		return (false);
	return mat->alpha > mat1->alpha || (mat->texture_albedo && mat1->texture_albedo &&
		mat->texture_albedo->bpp() <= 24 && mat1->texture_albedo->bpp() >= 32);
}

void	Renderable::alpha_sort()
{
	Engine::sort(alpha_compare);
}

Mesh	*Mesh::create(const std::string &name)
{
	Mesh *m = new Mesh(name);
	Engine::add(*m);
	return (m);
}

template <typename T>
static GLuint	vbuffer_load(GLuint attrib, int size, const std::vector<T> &a)
{
	GLuint	lbufferid;

	if (!size || !a.size())
		return (-1);
	glGenBuffers(1, &lbufferid);
	glBindBuffer(GL_ARRAY_BUFFER, lbufferid);
	glBufferData(GL_ARRAY_BUFFER, a.size() * sizeof(T), &a[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, size,
		sizeof(T) == 4 ? GL_UNSIGNED_BYTE : GL_FLOAT,
		GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return (lbufferid);
}

void			Mesh::load()
{
	if (is_loaded())
		return ;
	if (glIsVertexArray(v_arrayid))
		glDeleteVertexArrays(1, &v_arrayid);
	glGenVertexArrays(1, &v_arrayid);
	glBindVertexArray(v_arrayid);
	if (glIsBuffer(v_bufferid))
		glDeleteBuffers(1, &v_bufferid);
	v_bufferid = vbuffer_load(0, 3, v);
	if (glIsBuffer(vn_bufferid))
		glDeleteBuffers(1, &v_bufferid);
	vn_bufferid = vbuffer_load(1, 4, vn);
	if (glIsBuffer(vt_bufferid))
		glDeleteBuffers(1, &v_bufferid);
	vt_bufferid = vbuffer_load(2, 2, vt);
	glBindVertexArray(0);
	_is_loaded = true;
}

void	display_mat4(MAT4 &m)
{
	for (unsigned i = 0; i < 4; i++)
	{
		for (unsigned j = 0; j < 4; j++)
			std::cout << m.m[i + j * 4] << " ";
		std::cout << "\n";
	}
	std::cout << std::endl;
}

void		Mesh::bind()
{
	MAT4		mvp;
	MAT4		normal_matrix;

	if (!material || !material->shader)
		return ;
	mvp = mat4_combine(Engine::current_camera()->projection, Engine::current_camera()->view, mat4_transform());
	normal_matrix = mat4_transpose(mat4_inverse(mat4_transform()));
	material->shader->set_uniform("in_UVMax", uvmax);
	material->shader->set_uniform("in_UVMin", uvmin);
	material->shader->set_uniform("in_CamPos", Engine::current_camera()->position());
	material->shader->set_uniform("in_Transform", mvp);
	material->shader->set_uniform("in_ModelMatrix", mat4_transform());
	material->shader->set_uniform("in_NormalMatrix", normal_matrix);
}

void			Mesh::render()
{
	if (!material || !material->shader)
		return ;
	material->shader->use();
	material->load_textures();
	material->bind_textures();
	material->bind_values();
	load();
	bind();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(v_arrayid);
	glDrawArrays(GL_TRIANGLES, 0, v.size());
	glBindVertexArray(0);
	//material->shader->use(false);
}

void			Mesh::center()
{
	for (auto &vec : v)
	{
		vec = vec3_sub(vec, bounding_element->center);
		bounding_element->min.x = std::min(vec.x, bounding_element->min.x);
		bounding_element->min.y = std::min(vec.y, bounding_element->min.y);
		bounding_element->min.z = std::min(vec.z, bounding_element->min.z);
		bounding_element->max.x = std::max(vec.x, bounding_element->max.x);
		bounding_element->max.y = std::max(vec.y, bounding_element->max.y);
		bounding_element->max.z = std::max(vec.z, bounding_element->max.z);
	}
	bounding_element->center = vec3_scale(vec3_add(bounding_element->min, bounding_element->max), 0.5f);
	int i = 0;
	for (auto child : children)
	{
		i++;
		auto	m = static_cast<Mesh *>(child);
		auto	mPos = m->bounding_element->center;
		m->center();
		m->position() = vec3_sub(mPos, bounding_element->center);
	}
}
