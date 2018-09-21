/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 15:30:58 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

class Vgroup;

typedef bool (*renderable_compare)(Renderable *m, Renderable *m1);

class	Mesh : public Renderable
{
public:
	static std::shared_ptr<Mesh>	create(const std::string &);
	static std::shared_ptr<Mesh>	get_by_name(const std::string &);
    void							load();
	bool							render(RenderMod mod = RenderAll);
	bool							render_depth(RenderMod mod = RenderAll);
	void							center();
	void							set_cull_mod(GLenum);
	void							add(std::shared_ptr<Vgroup>);
private:
	Mesh(const std::string &name);
	static std::vector<std::shared_ptr<Mesh>>	_meshes;
	std::vector<std::shared_ptr<Vgroup>>		_vgroups;
	GLenum							_cull_mod{GL_BACK};
};

bool	alpha_compare(Renderable	*m, Renderable *m1);
