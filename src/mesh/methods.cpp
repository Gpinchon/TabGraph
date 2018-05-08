/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:32:34 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/05 14:19:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"

Mesh::Mesh(const std::string &name) : Node(name), material(nullptr), v(std::vector<VEC3>(0)),  _is_loaded(false)
{
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

bool			Mesh::is_loaded()
{
	return (_is_loaded);
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

	if (!material)
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
	std::cout << "Mesh::center " << _name << std::endl;
	std::cout << "parent " << parent << std::endl;
	for (auto &vec : v)
	{
		if (parent)
			vec = vec3_sub(vec, parent->bounding_element.center);
		else
			vec = vec3_sub(vec, bounding_element.center);
		bounding_element.min.x = MIN(vec.x, bounding_element.min.x);
		bounding_element.min.y = MIN(vec.y, bounding_element.min.y);
		bounding_element.min.z = MIN(vec.z, bounding_element.min.z);
		bounding_element.max.x = MAX(vec.x, bounding_element.max.x);
		bounding_element.max.y = MAX(vec.y, bounding_element.max.y);
		bounding_element.max.z = MAX(vec.z, bounding_element.max.z);
	}
	bounding_element.center = vec3_scale(vec3_add(bounding_element.min, bounding_element.max), 0.5f);
	position() = bounding_element.center;
	int i = 0;
	for (auto child : children)
	{
		std::cout << "Updating child " << i << std::endl;
		i++;
		static_cast<Mesh *>(child)->center();
	}
		
}
