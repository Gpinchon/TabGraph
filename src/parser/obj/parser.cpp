/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/06 00:03:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Vgroup.hpp"
#include "parser/InternalTools.hpp"
#include "parser/MTLLIB.hpp"
#include "parser/OBJ.hpp"
#include <stdexcept>

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
			parse_v(p, lesplit, nullptr);
		}
		i++;
	}
}

static void	parse_line(t_obj_parser *p, const char *line)
{
	auto split = strsplitwspace(line);
	if (split.empty() || split[0][0] == '#') {
		return ;
}
	if (split[0][0] == 'v') {
		parse_vtn(p, split);
	} else if (split[0][0] == 'f') {
		parse_f(p, split);
	} else if (split[0][0] == 'g' || split[0][0] == 'o'
		|| split[0] == "usemtl")
	{
		parse_vg(p);
		auto	mtl = Material::get_by_name(split[1]);
		if (mtl != nullptr)
			p->vg->material = mtl;
	}
	else if (split[0] == "mtllib") {
		PBRMTLLIB::parse(p->path_split[0] + split[1]);
	}
}


static void	start_obj_parsing(t_obj_parser *p, const std::string &name, const std::string& path)
{
	char	line[4096];

	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
	}
	if ((p->fd = fopen(path.c_str(), "r")) == nullptr) {
		throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
	}
	static auto	defaultMat = Material::get_by_name("default");
	p->parent = Mesh::create(name);
	p->vg = Vgroup::create(name + "_child 0");//new Vgroup(name + "_child 0");//Mesh::create(name + "_child 0");
	p->vg->material = defaultMat;
	p->vg->bounding_element = new AABB(p->bbox);
	while (fgets(line, 4096, p->fd) != nullptr) {
		parse_line(p, line);
	}
	fclose(p->fd);
	if (!p->vg->v.empty() != 0u) {
		parse_vg(p);
	}
	else {
		throw std::runtime_error(std::string("Invalid OBJ"));
	}
}

Mesh	*OBJ::parse(const std::string &name, const std::string &path)
{
	t_obj_parser	p;

	p.path_split = split_path(path);
	try {
		start_obj_parsing(&p, name, path);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error parsing ") + name + " :\n" + e.what());
		return (nullptr);
	}
	p.parent->bounding_element = new AABB(p.bbox);
	return (p.parent);
}
