/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/11/15 21:30:14 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Renderable.hpp"

class Vgroup;

class	Mesh : public Renderable
{
public:
	static std::shared_ptr<Mesh>	create(const std::string &);
	static std::shared_ptr<Mesh>	get_by_name(const std::string &);
	static std::shared_ptr<Mesh>	get(unsigned index);
	static void						add(std::shared_ptr<Mesh>);
    void							load();
	bool							render(RenderMod mod = RenderAll);
	bool							render_depth(RenderMod mod = RenderAll);
	void							center();
	void							set_cull_mod(GLenum);
	void							add(std::shared_ptr<Vgroup>);
	std::shared_ptr<Vgroup>			vgroup(unsigned index);
private:
	Mesh(const std::string &name);
	static std::vector<std::shared_ptr<Mesh>>	_meshes;
	std::vector<std::weak_ptr<Vgroup>>		_vgroups;
	GLenum							_cull_mod{GL_BACK};
};

bool	alpha_compare(Renderable	*m, Renderable *m1);
