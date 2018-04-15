/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:45:06 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/15 16:52:14 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parser.h"

static void	vt_min_max(t_vgroup *vg)
{
	unsigned	i;

	vg->uvmin = new_vec2(100000, 100000);
	vg->uvmax = new_vec2(-100000, -100000);
	i = 0;
	while (i < vg->vt.length)
	{
		VEC2	v = *((VEC2 *)ezarray_get_index(vg->vt, i));
		if (v.x < vg->uvmin.x)
			vg->uvmin.x = v.x;
		if (v.y < vg->uvmin.y)
			vg->uvmin.y = v.y;
		if (v.x > vg->uvmax.x)
			vg->uvmax.x = v.x;
		if (v.y > vg->uvmax.y)
			vg->uvmax.y = v.y;
		i++;
	}
}

void	parse_vg(t_obj_parser *p)
{
	if (p->vg.v.length > 0)
	{
		ezarray_shrink(&p->vg.v);
		ezarray_shrink(&p->vg.vn);
		ezarray_shrink(&p->vg.vt);
		vt_min_max(&p->vg);
		ezarray_push(&p->mesh.vgroups, &p->vg);
		p->vg = new_vgroup();
	}
}

void	correct_vt(VEC2 *vt)
{
	VEC3		v[3];
	VEC3		texnormal;

	v[0] = vec2_to_vec3(vt[0], 0);
	v[1] = vec2_to_vec3(vt[1], 0);
	v[2] = vec2_to_vec3(vt[2], 0);
	texnormal = vec3_cross(vec3_sub(v[1], v[0]), vec3_sub(v[2], v[0]));
	if (texnormal.z > 0)
	{
		if (vt[0].x < 0.25f)
			vt[0].x += 1.f;
		if (vt[1].x < 0.25f)
			vt[1].x += 1.f;
		if (vt[2].x < 0.25f)
			vt[2].x += 1.f;
	}
}

VEC2	generate_vt(VEC3 v, VEC3 center)
{
	VEC2		vt;
	VEC3		vec;

	vec = vec3_normalize(vec3_sub(center, v));
	vt.x = 0.5f + (atan2(vec.z, vec.x) / (2 * M_PI));
	vt.y = 0.5f + -vec.y * 0.5f;
	return (vt);
}

VEC3	generate_vn(VEC3 *v)
{
	return (vec3_normalize(vec3_cross(vec3_sub(v[1], v[0]), vec3_sub(v[2], v[0]))));
}
