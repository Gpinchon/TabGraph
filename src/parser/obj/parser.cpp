/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 01:03:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "parser/OBJ.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>

static void	parse_f(t_obj_parser *p, std::vector<std::string> &split)
{
	short	faces;
	short	i;
	t_vec2	v[0];

	(void)v;
	split.erase(split.begin());
	faces = split.size() - 3 + 1;
	i = 0;
	while (i < faces)
	{
		if (faces == 2 && i == 0)
		{
			auto lesplit = std::vector<std::string>({split[0], split[i + 1], split[i + 2]});
			auto levector = std::vector<VEC2>({new_vec2(0, 0), new_vec2(0, 1), new_vec2(1, 1)});
			parse_v(p, lesplit, &levector[0]);
		}
		else if (faces == 2 && i >= 1)
		{
			auto lesplit = std::vector<std::string>({split[0], split[i + 1], split[i + 2]});
			auto levector = std::vector<VEC2>({new_vec2(0, 0), new_vec2(1, 1), new_vec2(1, 0)});
			parse_v(p, lesplit, &levector[0]);
		}
		else
		{
			auto lesplit = std::vector<std::string>({split[0], split[i + 1], split[i + 2]});
			parse_v(p, lesplit, NULL);
		}
		i++;
	}
}

static void	parse_line(t_obj_parser *p, const char *line)
{
	auto split = strsplitwspace((const char *)line);
	if (!split.size() || split[0][0] == '#')
		return ;
	if (split[0][0] == 'v')
		parse_vtn(p, split);
	else if (split[0][0] == 'f')
		parse_f(p, split);
	else if (split[0][0] == 'g' || split[0][0] == 'o'
		|| split[0] == "usemtl")
	{
		parse_vg(p);
		p->vg->material = Material::get_by_name(split[1]);
	}
	else if (split[0] == "mtllib")
		load_mtllib(p->path_split[0] + split[1]);
}

static bool	start_obj_parsing(t_obj_parser *p, const std::string &name, const std::string path)
{
	char	line[4096];

	if (access(path.c_str(), F_OK | R_OK) || !(p->fd = fopen(path.c_str(), "r")))
		return (false);
	p->parent = Mesh::create(name);
	p->vg = Mesh::create(name + "_child 0");
	p->vg->material = Material::get_by_name("default");
	p->vg->bounding_element = new AABB(p->bbox);
	while (fgets(line, 4096, p->fd))
		parse_line(p, line);
	if (p->vg->v.size())
		parse_vg(p);
	else
		return (false);
	return (true);
}

Mesh	*OBJ::parse(const std::string &name, const std::string &path)
{
	t_obj_parser	p;

	p.path_split = split_path(path);
	if (!Material::get_by_name("default"))
		PBRMaterial::create("default");
	if (!start_obj_parsing(&p, name, path))
		return (nullptr);
	p.parent->bounding_element = new AABB(p.bbox);
	return (p.parent);
}
