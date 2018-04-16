/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:46:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 17:59:32 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>

static void	push_values(t_obj_parser *p, VEC3 *v, VEC3 *vn, VEC2 *vt)
{
	unsigned		i;
	unsigned char	ub[4];

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

static int	get_vi(ARRAY v, const char *str)
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

static void	parse_indice(t_obj_parser *p, char **split, int vindex[3][3])
{
	char		**fsplit;
	unsigned	i[3];

	i[0] = 0;
	while (split[i[0]] && i[0] < 3)
	{
		fsplit = ft_strsplit(split[i[0]], '/');
		i[1] = ft_chartablen(fsplit);
		i[2] = count_char(split[i[0]], '/');
		vindex[0][i[0]] = -1;
		vindex[1][i[0]] = -1;
		vindex[2][i[0]] = -1;
		vindex[0][i[0]] = get_vi(p->v, fsplit[0]);
		if (vindex[0][i[0]] == -1)
			return ;
		if ((i[1] == 3 && i[2] == 2) || (i[1] == 2 && i[2] == 1))
			vindex[2][i[0]] = get_vi(p->vt, fsplit[1]);
		if (i[1] == 3 && i[2] == 2)
			vindex[1][i[0]] = get_vi(p->vn, fsplit[2]);
		else if (i[1] == 2 && i[2] == 2)
			vindex[1][i[0]] = get_vi(p->vn, fsplit[1]);
		ft_free_chartab(fsplit);
		i[0]++;
	}
}

static void	parse_vn(t_obj_parser *p, int vindex[3][3], VEC3 v[3], VEC3 vn[3])
{
	short	i;

	i = 0;
	while (i < 3)
	{
		if (vindex[1][i] != -1)
			vn[i] = *((VEC3*)ezarray_get_index(p->vn, vindex[1][i]));
		else
			vn[i] = generate_vn(v);
		i++;
	}
}

void		parse_v(t_obj_parser *p, char **split, VEC2 *in_vt)
{
	int		vindex[3][3];
	VEC3	v[3];
	VEC3	vn[3];
	VEC2	vt[3];
	short	i;

	parse_indice(p, split, vindex);
	i = -1;
	while (++i < 3)
	{
		if (vindex[0][i] == -1)
			return ;
		v[i] = *((VEC3*)ezarray_get_index(p->v, vindex[0][i]));
		if (vindex[2][i] != -1)
		{
			vt[i] = *((VEC2*)ezarray_get_index(p->vt, vindex[2][i]));
			in_vt = (VEC2 *)0x1;
		}
		else
			vt[i] = in_vt ? in_vt[i] : generate_vt(v[i], p->bbox.center);
	}
	parse_vn(p, vindex, v, vn);
	if (!in_vt)
		correct_vt(vt);
	push_values(p, v, vn, vt);
}
