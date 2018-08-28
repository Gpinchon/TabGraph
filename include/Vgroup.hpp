/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Vgroup.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:43:25 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/28 19:15:03 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

struct  CVEC4
{
	GLubyte	x;
	GLubyte	y;
	GLubyte	z;
	GLubyte	w;
};

class	VertexArray;

class	Vgroup : public Renderable
{
public:
	static Vgroup		*create(const std::string &);
	std::vector<VEC3>	v;
	std::vector<CVEC4>	vn;
	std::vector<VEC2>	vt;
	VEC2				uvmax{1, 1};
	VEC2				uvmin{0, 0};
	void				bind();
	void				load();
	bool				render(RenderMod mod = RenderAll);
	void				center(VEC3 &center);
private:
	Vgroup(const std::string &);
	VertexArray			*_vao{nullptr};
};