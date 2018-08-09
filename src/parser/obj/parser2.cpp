/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:45:06 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/09 19:02:11 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Vgroup.hpp"
#include "Material.hpp"
#include "parser/InternalTools.hpp"
#include "parser/OBJ.hpp"

static void	vt_min_max(Vgroup *vg)
{
	vg->uvmin = new_vec2(100000, 100000);
	vg->uvmax = new_vec2(-100000, -100000);
	for (auto &vt : vg->vt)
	{
		if (vt.x < vg->uvmin.x) {
			vg->uvmin.x = vt.x;
		}
		if (vt.y < vg->uvmin.y) {
			vg->uvmin.y = vt.y;
		}
		if (vt.x > vg->uvmax.x) {
			vg->uvmax.x = vt.x;
		}
		if (vt.y > vg->uvmax.y) {
			vg->uvmax.y = vt.y;
		}
	}
	if (vg->uvmin.x == vg->uvmax.x)
	{
		vg->uvmin.x = 0;
		vg->uvmax.x = 1;
	}
	if (vg->uvmin.y == vg->uvmax.y)
	{
		vg->uvmin.y = 0;
		vg->uvmax.y = 1;
	}
}

void		parse_vg(t_obj_parser *p)
{
	static int	childNbr = 0;
	if (!p->vg->v.empty())
	{
		childNbr++;
		vt_min_max(p->vg);
		p->parent->add_child(*p->vg);
		p->parent->vgroups.push_back(p->vg);
		p->vg = Vgroup::create(p->parent->name() + "_child " + std::to_string(childNbr));
		p->vg->material = Material::get_by_name("default");
	}
}

void		correct_vt(VEC2 *vt)
{
	VEC3		v[3];
	VEC3		texnormal{0, 0, 0};

	v[0] = vec2_to_vec3(vt[0], 0);
	v[1] = vec2_to_vec3(vt[1], 0);
	v[2] = vec2_to_vec3(vt[2], 0);
	texnormal = vec3_cross(vec3_sub(v[1], v[0]), vec3_sub(v[2], v[0]));
	if (texnormal.z > 0)
	{
		if (vt[0].x < 0.25f) {
			vt[0].x += 1.f;
		}
		if (vt[1].x < 0.25f) {
			vt[1].x += 1.f;
		}
		if (vt[2].x < 0.25f) {
			vt[2].x += 1.f;
		}
	}
}

VEC2		generate_vt(VEC3 v, VEC3 center)
{
	VEC2		vt{0, 0};
	VEC3		vec{0, 0, 0};

	vec = vec3_normalize(vec3_sub(center, v));
	vt.x = 0.5f + (atan2(vec.z, vec.x) / (2 * M_PI));
	vt.y = 0.5f + -vec.y * 0.5f;
	return (vt);
}

VEC3		generate_vn(VEC3 *v)
{
	return (vec3_normalize(vec3_cross(vec3_sub(v[1], v[0]),
		vec3_sub(v[2], v[0]))));
}
