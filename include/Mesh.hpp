/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/03 23:53:50 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

class Vgroup;

typedef bool (*renderable_compare)(Renderable *m, Renderable *m1);

class	Mesh : public Renderable
{
public:
	static Mesh	*create(const std::string &);
	static Mesh	*get_by_name(const std::string &);
	void		sort(renderable_compare);
    void		load();
	bool		render(RenderMod mod = RenderAll);
	bool		render_depth(RenderMod mod = RenderAll);
	void		center();
	std::vector<Vgroup*>	vgroups;
	void		set_cull_mod(GLenum);
private:
	Mesh(const std::string &name);
	GLenum		_cull_mod{GL_BACK};
};

bool	alpha_compare(Renderable	*m, Renderable *m1);
