/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vgroup.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:47:26 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/10 17:11:49 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

static GLuint	vbuffer_load(GLuint attrib, int size, ARRAY a)
{
	GLuint	lbufferid;
	if (!size || !a.length)
		return (-1);
	glGenBuffers(1, &lbufferid);
	glBindBuffer(GL_ARRAY_BUFFER, lbufferid);
	glBufferData(GL_ARRAY_BUFFER, a.total_size, a.data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, size,
		a.data_size == 4 ? GL_UNSIGNED_BYTE : GL_FLOAT,
		//(a.data_size == sizeof(VEC3) || a.data_size == sizeof(VEC2)) ? GL_FLOAT : GL_UNSIGNED_BYTE,
		GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return (lbufferid);
}

void	vgroup_load(int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;

	if (!(mesh = ezarray_get_index(engine_get()->meshes, mesh_index)))
		return;
	if (!(vgroup = ezarray_get_index(mesh->vgroups, vgroup_index)))
		return;
	if (glIsVertexArray(vgroup->v_arrayid))
		glDeleteVertexArrays(1, &vgroup->v_arrayid);
	glGenVertexArrays(1, &vgroup->v_arrayid);
	glBindVertexArray(vgroup->v_arrayid);
	if (glIsBuffer(vgroup->v_bufferid))
		glDeleteBuffers(1, &vgroup->v_bufferid);
	vgroup->v_bufferid = vbuffer_load(0, 3, vgroup->v);
	if (glIsBuffer(vgroup->vn_bufferid))
		glDeleteBuffers(1, &vgroup->v_bufferid);
	vgroup->vn_bufferid = vbuffer_load(1, 4, vgroup->vn);
	if (glIsBuffer(vgroup->vt_bufferid))
		glDeleteBuffers(1, &vgroup->v_bufferid);
	vgroup->vt_bufferid = vbuffer_load(2, 2, vgroup->vt);
	glBindVertexArray(0);
}

void	vgroup_render(int camera_index, int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;
	t_material	*material;
	t_camera	*camera;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	camera = ezarray_get_index(engine_get()->cameras, camera_index);
	if (!mesh || !vgroup || !camera)
		return;
	material = ezarray_get_index(engine_get()->materials, vgroup->mtl_index);
	if (!material)
		return;
	material_load_textures(vgroup->mtl_index);
	t_transform *t = ezarray_get_index(engine_get()->transforms, mesh->transform_index);

	MAT4 transform;
	transform = mat4_combine(camera->projection, camera->view, t->transform);
	MAT4	normal_matrix = mat4_transpose(mat4_inverse(t->transform));
	shader_use(material->shader_index);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	material_set_uniforms(vgroup->mtl_index);
	shader_set_uniform(material->shader_index, material->in_campos, &camera->position);
	shader_set_uniform(material->shader_index, material->in_modelmatrix, &t->transform);
	shader_set_uniform(material->shader_index, material->in_normalmatrix, &normal_matrix);
	shader_set_uniform(material->shader_index, material->in_transform, &transform);
	shader_set_uniform(material->shader_index, material->in_uvmin, &vgroup->uvmin);
	shader_set_uniform(material->shader_index, material->in_uvmax, &vgroup->uvmax);
	glBindVertexArray(vgroup->v_arrayid);
	glDrawArrays(GL_TRIANGLES, 0, vgroup->v.length);
	glBindVertexArray(0);
	glUseProgram(0);
}

void	vgroup_center(int mesh_index, int vgroup_index)
{
	t_mesh		*mesh;
	t_vgroup	*vgroup;
	VEC3		*v;
	unsigned	v_index;

	mesh = ezarray_get_index(engine_get()->meshes, mesh_index);
	vgroup = ezarray_get_index(mesh->vgroups, vgroup_index);
	if (!vgroup)
		return;
	v_index = 0;
	while (v_index < vgroup->v.length)
	{
		v = ezarray_get_index(vgroup->v, v_index);
		*v = vec3_sub(*v, mesh->bounding_box.center);
		v_index++;
	}
}