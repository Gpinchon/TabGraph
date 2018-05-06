/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser3.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:46:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/01 09:58:49 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>

static void	push_values(t_obj_parser *p, VEC3 *v, VEC3 *vn, VEC2 *vt)
{
	unsigned		i;
	CVEC4			ub;

	i = 0;
	while (i < 3)
	{
		p->vg->bounding_element.min.x = MIN(v[i].x, p->vg->bounding_element.min.x);
		p->vg->bounding_element.min.y = MIN(v[i].y, p->vg->bounding_element.min.y);
		p->vg->bounding_element.min.z = MIN(v[i].z, p->vg->bounding_element.min.z);
		p->vg->bounding_element.max.x = MAX(v[i].x, p->vg->bounding_element.max.x);
		p->vg->bounding_element.max.y = MAX(v[i].y, p->vg->bounding_element.max.y);
		p->vg->bounding_element.max.z = MAX(v[i].z, p->vg->bounding_element.max.z);
		p->vg->bounding_element.center = vec3_scale(vec3_add(p->vg->bounding_element.min,
			p->vg->bounding_element.max), 0.5);
		p->vg->v.push_back(v[i]);
		p->vg->vt.push_back(vt[i]);
		ub.x = (vn[i].x + 1) * 0.5 * 255;
		ub.y = (vn[i].y + 1) * 0.5 * 255;
		ub.z = (vn[i].z + 1) * 0.5 * 255;
		ub.w = 255;
		p->vg->vn.push_back(ub);
		i++;
	}
}

static int	get_vi(const std::vector<VEC2> &v, const std::string &str)
{
	int	vindex;

	vindex = std::stoi(str);
	if (vindex < 0)
		vindex = v.size() + vindex;
	else
		vindex -= 1;
	if (vindex < 0 || (unsigned)vindex >= v.size())
		return (-1);
	return (vindex);
}

static int	get_vi(const std::vector<VEC3> &v, const std::string &str)
{
	int	vindex;

	vindex = std::stoi(str);
	if (vindex < 0)
		vindex = v.size() + vindex;
	else
		vindex -= 1;
	if (vindex < 0 || (unsigned)vindex >= v.size())
		return (-1);
	return (vindex);
}

static void	parse_indice(t_obj_parser *p, std::vector<std::string> &split, int vindex[3][3])
{
	unsigned					i[3];

	i[0] = 0;
	while (i[0] < split.size() && i[0] < 3)
	{
		auto fsplit = strsplit(split[i[0]], '/');
		i[1] = fsplit.size();
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
			vn[i] = p->vn[vindex[1][i]];
		else
			vn[i] = generate_vn(v);
		i++;
	}
}

void		parse_v(t_obj_parser *p, std::vector<std::string> &split, VEC2 *in_vt)
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
		v[i] = p->v[vindex[0][i]];
		if (vindex[2][i] != -1)
		{
			vt[i] = p->vt[vindex[2][i]];
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
