/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:42:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 17:21:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>

t_vgroup	new_vgroup(void)
{
	t_vgroup	vg;

	memset(&vg, 0, sizeof(t_vgroup));
	vg.mtl_id = hash((unsigned char*)"default");
	vg.bounding_box.min = new_vec3(100000, 100000, 100000);
	vg.bounding_box.max = new_vec3(-100000, -100000, -100000);
	vg.v = new_ezarray(other, 0, sizeof(VEC3));
	vg.vn = new_ezarray(other, 0, 4 * sizeof(UCHAR));
	vg.vt = new_ezarray(other, 0, sizeof(VEC2));
	return (vg);
}

t_mesh		new_mesh(void)
{
	t_mesh		m;

	memset(&m, 0, sizeof(t_mesh));
	m.vgroups = new_ezarray(other, 0, sizeof(t_vgroup));
	return (m);
}

VEC3		parse_vec3(char **split)
{
	float		v[3];
	int			i;

	i = 0;
	memset(v, 0, sizeof(float) * 3);
	while (split[i] && i < 3)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec3(v[0], v[1], v[2]));
}

VEC2		parse_vec2(char **split)
{
	float		v[2];
	int			i;

	i = 0;
	memset(v, 0, sizeof(float) * 2);
	while (split[i] && i < 2)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec2(v[0], v[1]));
}

void		parse_vtn(t_obj_parser *p, char **split)
{
	VEC3	v;
	VEC2	vn;

	if (!strcmp(split[0], "v"))
	{
		v = parse_vec3(&split[1]);
		p->bbox.min.x = v.x < p->bbox.min.x ? v.x : p->bbox.min.x;
		p->bbox.min.y = v.y < p->bbox.min.y ? v.y : p->bbox.min.y;
		p->bbox.min.z = v.z < p->bbox.min.z ? v.z : p->bbox.min.z;
		p->bbox.max.x = v.x > p->bbox.max.x ? v.x : p->bbox.max.x;
		p->bbox.max.y = v.y > p->bbox.max.y ? v.y : p->bbox.max.y;
		p->bbox.max.z = v.z > p->bbox.max.z ? v.z : p->bbox.max.z;
		p->bbox.center = vec3_fdiv(vec3_add(p->bbox.min, p->bbox.max), 2);
		ezarray_push(&p->v, &v);
	}
	else if (!strcmp(split[0], "vn"))
	{
		v = parse_vec3(&split[1]);
		ezarray_push(&p->vn, &v);
	}
	else if (!strcmp(split[0], "vt"))
	{
		vn = parse_vec2(&split[1]);
		ezarray_push(&p->vt, &vn);
	}
}
