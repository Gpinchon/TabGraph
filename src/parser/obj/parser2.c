/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser2.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:45:06 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/10 18:54:10 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parser.h"

static void	vt_min_max(t_vgroup *vg)
{
	unsigned	i = 0;
	vg->uvmin = new_vec2(100000, 100000);
	vg->uvmax = new_vec2(-100000, -100000);

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

void	parse_vg(t_obj_parser *p, char **split)
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
	(void)split;
}