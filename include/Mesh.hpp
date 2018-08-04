/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 00:36:04 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Vgroup.hpp" //WHY IS THIS NEEDED HERE ?!
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
	void		bind();
	void		render();
	void		center();
	std::vector<Vgroup*>	vgroups;
private:
	Mesh(const std::string &name);
};

bool	alpha_compare(Renderable	*m, Renderable *m1);
