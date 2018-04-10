/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:46:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/10 19:11:56 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parser.h"

void	push_values(t_obj_parser *p, VEC3 *v, VEC3 *vn, VEC2 *vt)
{
	unsigned	i;
	unsigned char ub[4];

	i = 0;
	while (i < 3)
	{
		p->vg.bounding_box.min.x = v[i].x < p->vg.bounding_box.min.x ? v[i].x :
		p->vg.bounding_box.min.x;
		p->vg.bounding_box.min.y = v[i].y < p->vg.bounding_box.min.y ? v[i].y :
		p->vg.bounding_box.min.y;
		p->vg.bounding_box.min.z = v[i].z < p->vg.bounding_box.min.z ? v[i].z :
		p->vg.bounding_box.min.z;
		p->vg.bounding_box.max.x = v[i].x > p->vg.bounding_box.max.x ? v[i].x :
		p->vg.bounding_box.max.x;
		p->vg.bounding_box.max.y = v[i].y > p->vg.bounding_box.max.y ? v[i].y :
		p->vg.bounding_box.max.y;
		p->vg.bounding_box.max.z = v[i].z > p->vg.bounding_box.max.z ? v[i].z :
		p->vg.bounding_box.max.z;
		p->vg.bounding_box.center = vec3_scale(vec3_add(p->vg.bounding_box.min,
			p->vg.bounding_box.max), 0.5);
		ezarray_push(&p->vg.v, &v[i]);
		ezarray_push(&p->vg.vt, &vt[i]);
		ub[0] = (vn[i].x + 1) * 0.5 * 255;
		ub[1] = (vn[i].y + 1) * 0.5 * 255;
		ub[2] = (vn[i].z + 1) * 0.5 * 255;
		ub[3] = 255;
		ezarray_push(&p->vg.vn, &ub);
		i++;
	}
}

void	parse_values(t_obj_parser *p, char **split, VEC3 *v, VEC3 *vn, VEC2 *vt)
{
	short	i;
	short	tablen;
	short	slash;
	char	**fsplit;
	VEC2	*uv;
	VEC3	*normal;

	i = 0;
	while (split[i])
	{
		fsplit = ft_strsplit(split[i], '/');
		slash = count_char(split[i], '/');
		tablen = ft_chartablen(fsplit);
		vn[i] = vec3_normalize(vec3_cross(vec3_sub(v[1], v[0]), vec3_sub(v[2], v[0])));
		if (tablen == 3 && slash == 2)
		{
			uv = ezarray_get_index(p->vt, atoi(fsplit[1]) - 1);
			vt[i] = uv ? *uv : vt[i];
			normal = ezarray_get_index(p->vn, atoi(fsplit[2]) - 1);
			vn[i] = normal ? *normal : vn[i];
		}
		else if (tablen == 2 && slash == 2)
		{
			normal = ezarray_get_index(p->vn, atoi(fsplit[1]) - 1);
			vn[i] = normal ? *normal : vn[i];
		}
		else if (tablen == 2 && slash == 1)
		{
			uv = ezarray_get_index(p->vt, atoi(fsplit[1]) - 1);
			vt[i] = uv ? *uv : vt[i];
		}
		ft_free_chartab(fsplit);
		i++;
	}
}

void	parse_v(t_obj_parser *p, char **split, VEC2 *in_vt)
{
	VEC3	v[3];
	VEC3	vn[3];
	VEC2	vt[3];
	int		i;
	int		vindex[3];
	char	**fsplit;

	i = 0;
	while (split[i])
	{
		fsplit = ft_strsplit(split[i], '/');
		vindex[i] = atoi(fsplit[0]);
		if (vindex[i] < 0)
			vindex[i] = p->v.length + vindex[i];
		else
			vindex[i] -= 1;
		if (vindex[i] < 0 || (unsigned)vindex[i] >= p->v.length)
			return;
		v[i] = *((VEC3*)ezarray_get_index(p->v, vindex[i]));
		if (!in_vt)
		{
			VEC3 vec = vec3_normalize(vec3_sub(p->bbox.center, v[i]));
			vt[i].x = 0.5f + (atan2(vec.z, vec.x) / (2 * M_PI));
			vt[i].y = -vec.y * 0.5f + 0.5f;
		}
		else
			vt[i] = in_vt[i];
		ft_free_chartab(fsplit);
		i++;
	}
	if (!in_vt)
	{
		VEC3	texa, texb, texc;
		texa = vec2_to_vec3(vt[0], 0);
		texb = vec2_to_vec3(vt[1], 0);
		texc = vec2_to_vec3(vt[2], 0);
		VEC3	texnormal;
		texnormal = vec3_cross(vec3_sub(texb, texa), vec3_sub(texc, texa));
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
	parse_values(p, split, v, vn, vt);
	push_values(p, v, vn, vt);
}
