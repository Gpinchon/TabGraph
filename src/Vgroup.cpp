/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vgroup.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:48:07 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/24 19:36:10 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Camera.hpp"
#include "Vgroup.hpp"
#include "VertexArray.hpp"
#include "Material.hpp"
#include "Shader.hpp"
#include "AABB.hpp"

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
	_vao->add_buffer(0, GL_FLOAT, 3, v);
	_vao->add_buffer(1, GL_UNSIGNED_BYTE, 4, vn);
	_vao->add_buffer(2, GL_FLOAT, 2, vt);
	_is_loaded = true;

	/*if (glIsVertexArray(v_arrayid)) {
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
	glBindVertexArray(0);*/
	
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

void			Vgroup::render(RenderMod mod)
{
	if ((material == nullptr) || (material->shader == nullptr)) {
		return ;
	}
	if (mod == RenderOpaque && (material->alpha < 1 || (material->texture_albedo != nullptr && material->texture_albedo->values_per_pixel() == 4)))
		return ;
	else if (mod == RenderTransparent && (material->alpha == 1 || (material->texture_albedo != nullptr && material->texture_albedo->values_per_pixel() < 4)))
		return ;
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
	//glBindVertexArray(v_arrayid);
	//_vao->bind();
	_vao->draw();
	//glDrawArrays(GL_TRIANGLES, 0, v.size());
	glBindVertexArray(0);
	material->shader->use(false);
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
