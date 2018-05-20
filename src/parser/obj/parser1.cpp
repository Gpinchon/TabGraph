/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:42:37 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/28 17:11:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/InternalTools.hpp"
#include "parser/OBJ.hpp"

VEC3		parse_vec3(std::vector<std::string> &split)
{
	float		v[3];
	unsigned	i;

	i = 0;
	memset(v, 0, sizeof(float) * 3);
	while (i < 3)
	{
		if ((i + 1) >= split.size()) {
			break ;
}
		v[i] = std::stof(split[i + 1]);
		i++;
	}
	return (new_vec3(v[0], v[1], v[2]));
}

VEC2		parse_vec2(std::vector<std::string> &split)
{
	float		v[2];
	unsigned	i;

	i = 0;
	memset(v, 0, sizeof(float) * 2);
	while (i < 2)
	{
		if ((i + 1) >= split.size()) {
			break ;
}
		v[i] = std::stof(split[i + 1]);
		i++;
	}
	return (new_vec2(v[0], v[1]));
}

void		parse_vtn(t_obj_parser *p, std::vector<std::string> &split)
{
	VEC3	v{};
	VEC2	vn{};

	if (split[0] == "v")
	{
		v = parse_vec3(split);
		p->bbox.min.x = std::min(v.x, p->bbox.min.x);
		p->bbox.min.y = std::min(v.y, p->bbox.min.y);
		p->bbox.min.z = std::min(v.z, p->bbox.min.z);
		p->bbox.max.x = std::max(v.x, p->bbox.max.x);
		p->bbox.max.y = std::max(v.y, p->bbox.max.y);
		p->bbox.max.z = std::max(v.z, p->bbox.max.z);
		p->bbox.center = vec3_fdiv(vec3_add(p->bbox.min, p->bbox.max), 2);
		p->v.push_back(v);
	}
	else if (split[0] == "vn")
	{
		v = parse_vec3(split);
		p->vn.push_back(v);
	}
	else if (split[0] == "vt")
	{
		vn = parse_vec2(split);
		p->vt.push_back(vn);
	}
}
