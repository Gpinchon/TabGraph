/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:46:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/12 21:55:26 by gpinchon         ###   ########.fr       */
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
		p->vg.bounding_box.min.x = MIN(v[i].x, p->vg.bounding_box.min.x);
		p->vg.bounding_box.min.y = MIN(v[i].y, p->vg.bounding_box.min.y);
		p->vg.bounding_box.min.z = MIN(v[i].z, p->vg.bounding_box.min.z);
		p->vg.bounding_box.max.x = MAX(v[i].x, p->vg.bounding_box.max.x);
		p->vg.bounding_box.max.y = MAX(v[i].y, p->vg.bounding_box.max.y);
		p->vg.bounding_box.max.z = MAX(v[i].z, p->vg.bounding_box.max.z);
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

int		get_vi(ARRAY v, const char *str)
{
	int	vindex;

	vindex = atoi(str);
	if (vindex < 0)
		vindex = v.length + vindex;
	else
		vindex -= 1;
	if (vindex < 0 || (unsigned)vindex >= v.length)
		return (-1);
	return (vindex);
}

void	parse_indice(t_obj_parser *p, char **split, int vindex[3][3])
{
	char		**fsplit;
	unsigned	i0;
	unsigned	i1;
	unsigned	i2;

	i0 = 0;
	while (split[i0] && i0 < 3)
	{
		fsplit = ft_strsplit(split[i0], '/');
		i1 = ft_chartablen(fsplit);
		i2 = count_char(split[i0], '/');
		vindex[0][i0] = -1;
		vindex[1][i0] = -1;
		vindex[2][i0] = -1;
		vindex[0][i0] = get_vi(p->v, fsplit[0]);
		if (vindex[0][i0] == -1)
			return ;
		if ((i1 == 3 && i2 == 2) || (i1 == 2 && i2 == 1))
			vindex[2][i0] = get_vi(p->vt, fsplit[1]);
		if (i1 == 3 && i2 == 2)
			vindex[1][i0] = get_vi(p->vn, fsplit[2]);
		else if (i1 == 2 && i2 == 2)
			vindex[1][i0] = get_vi(p->vn, fsplit[1]);
		ft_free_chartab(fsplit);
		i0++;
	}
}

void	parse_vn(t_obj_parser *p, int vindex[3][3], VEC3 v[3], VEC3 vn[3])
{
	short		i0;

	i0 = 0;
	while (i0 < 3)
	{
		if (vindex[1][i0] != -1)
			vn[i0] = *((VEC3*)ezarray_get_index(p->vn, vindex[1][i0]));
		else
			vn[i0] = generate_vn(v);
		i0++;
	}
}

void	parse_v(t_obj_parser *p, char **split, VEC2 *in_vt)
{
	int			vindex[3][3];
	VEC3		v[3];
	VEC3		vn[3];
	VEC2		vt[3];
	short		i0;

	parse_indice(p, split, vindex);
	i0 = 0;
	while (i0 < 3)
	{
		if (vindex[0][i0] != -1)
			v[i0] = *((VEC3*)ezarray_get_index(p->v, vindex[0][i0]));
		if (vindex[2][i0] != -1)
		{
			vt[i0] = *((VEC2*)ezarray_get_index(p->vt, vindex[2][i0]));
			in_vt = (VEC2 *)0x1;
		}
		else
			vt[i0] = in_vt ? in_vt[i0] : generate_vt(v[i0], p->bbox.center);
		i0++;
	}
	parse_vn(p, vindex, v, vn);
	if (!in_vt)
		correct_vt(vt);
	push_values(p, v, vn, vt);
}