/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/07/31 19:59:17 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include "Renderable.hpp"
#include "AABB.hpp"

struct  CVEC4
{
	GLubyte	x;
	GLubyte	y;
	GLubyte	z;
	GLubyte	w;
};

class	Vgroup
{
public:
	Vgroup();
	std::vector<VEC3>	v;
	std::vector<CVEC4>	vn;
	std::vector<VEC2>	vt;
	VEC2				uvmax{1, 1};
	VEC2				uvmin{0, 0};
	GLuint				v_arrayid{0};
	GLuint				v_bufferid{0};
	GLuint				vn_bufferid{0};
	GLuint				vt_bufferid{0};
	void				bind();
	void				load();
	void				render();
	void				center();
private:
	
	GLenum				_cull_mod{GL_BACK};
};

class	Mesh : public Renderable
{
public:
	static Mesh	*create(const std::string &);
	static Mesh	*get_by_name(const std::string &);
    void		load();
	void		bind();
	void		render();
	void		center();
	std::vector<Vgroup*>	vgroups;
private:
	Mesh(const std::string &name);
};
