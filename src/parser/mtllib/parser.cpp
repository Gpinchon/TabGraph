/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/09 18:55:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "Material.hpp"
#include "parser/BMP.hpp"
#include "parser/InternalTools.hpp"
#include "parser/MTLLIB.hpp"
#include <stdexcept>
#include <unistd.h>

void	parse_color(std::vector<std::string> &split, Material *mtl)
{
	if (split[0] == "Kd") {
		mtl->albedo = parse_vec3(split);
	}
	else if (split[0] == "Ks") {
		mtl->specular = vec3_fdiv(parse_vec3(split),
			1 + (1 - mtl->metallic) * 24);
	}	
	else if (split[0] == "Ke") {
		mtl->emitting = parse_vec3(split);
	}
		
}

void	parse_texture(t_obj_parser *p, std::vector<std::string> &split, Material *mtl)
{
	std::string path(p->path_split[0]);

	path += split[1];
	if (split[0] == "map_Kd") {
		mtl->texture_albedo = BMP::parse(path, path);
	} else if (split[0] == "map_Ks") {
		mtl->texture_specular = BMP::parse(path, path);
	} else if (split[0] == "map_Ke") {
		mtl->texture_emitting = BMP::parse(path, path);
	} else if (split[0] == "map_Nh") {
		mtl->texture_height = BMP::parse(path, path);
	} else if (split[0] == "map_No") {
		mtl->texture_ao = BMP::parse(path, path);
	} /*else if (split[0] == "map_Ns") {
		mtl->texture_sss = BMP::parse(path, path);
	}*/ else if (split[0] == "map_Nr") {
		mtl->texture_roughness = BMP::parse(path, path);
	} else if (split[0] == "map_Nm") {
		mtl->texture_metallic = BMP::parse(path, path);
	} else if (split[0] == "map_bump" || split[0] == "map_Bump") {
		mtl->texture_normal = BMP::parse(path, path);
	}
}

void	parse_number(std::vector<std::string> &split, Material *mtl)
{
	float ior;

	if (split[0] == "Np") {
		mtl->parallax = std::stof(split[1]);
	} else if (split[0] == "Ns") {
		mtl->roughness = CLAMP(1.f / (1.f + std::stof(split[1])) * 50.f, 0, 1);
	} else if (split[0] == "Nr") {
		mtl->roughness = std::stof(split[1]);
	} else if (split[0] == "Nm") {
		mtl->metallic = std::stof(split[1]);
	} else if (split[0] == "Ni")
	{
		ior = std::stof(split[1]);
		ior = (ior - 1) / (ior + 1);
		ior *= ior;
		mtl->specular = new_vec3(ior, ior, ior);
	}
	else if (split[0] == "Tr") {
		mtl->alpha = 1 - std::stof(split[1]);
	}
}

void	parse_mtl(t_obj_parser *p, std::string &name)
{
	Material	*mtl;
	char		line[4096];

	if (Material::get_by_name(name) != nullptr) {
		return ;
	}
	mtl = Material::create(name);
	while (fgets(line, 4096, p->fd) != nullptr)
	{
		auto msplit = strsplitwspace(line);
		if (msplit.size() > 1 && msplit[0][0] != '#')
		{
			if (msplit[0][0] == 'K') {
				parse_color(msplit, mtl);
			} else if (msplit[0][0] == 'N' || msplit[0][0] == 'T') {
				parse_number(msplit, mtl);
			} else if (msplit[0].find("map_") != std::string::npos) {
				parse_texture(p, msplit, mtl);
			} else if (msplit[0] == "newmtl") {
				parse_mtl(p, msplit[1]);
	}
		}
	}
}

bool			PBRMTLLIB::parse(const std::string &path)
{
	char		line[4096];
	t_obj_parser p;

	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
	}
	if ((p.fd = fopen(path.c_str(), "r")) == nullptr) {
		throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
	}
	p.path_split = split_path(path);
	while (fgets(line, 4096, p.fd) != nullptr)
	{
		auto split = strsplitwspace(line);
		if (split.size() > 1 && split[0][0] != '#')
		{
			if (split[0] == "newmtl") {
				parse_mtl(&p, split[1]);
			}
		}
	}
	fclose(p.fd);
	return (true);
}
