/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 01:03:06 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>
#include <unistd.h>

void	parse_color(std::vector<std::string> &split, PBRMaterial *mtl)
{
	if (split[0] == "Kd")
	{
		//split.erase(split.begin());
		mtl->albedo = parse_vec3(split);
	}
		
	else if (split[0] == "Ks")
	{
		//split.erase(split.begin());
		mtl->specular = vec3_fdiv(parse_vec3(split),
			1 + (1 - mtl->metallic) * 24);
	}	
	else if (split[0] == "Ke")
	{
		//split.erase(split.begin());
		mtl->emitting = parse_vec3(split);
	}
		
}

void	parse_texture(t_obj_parser *p, std::vector<std::string> &split, PBRMaterial *mtl)
{
	std::string path(p->path_split[0]);

	path += split[1];
	if (split[0] == "map_Kd")
		mtl->texture_albedo = BMP::parse(path, path);
	else if (split[0] == "map_Ks")
		mtl->texture_specular = BMP::parse(path, path);
	else if (split[0] == "map_Ke")
		mtl->texture_emitting = BMP::parse(path, path);
	else if (split[0] == "map_Nh")
		mtl->texture_height = BMP::parse(path, path);
	else if (split[0] == "map_No")
		mtl->texture_ao = BMP::parse(path, path);
	else if (split[0] == "map_Nr")
		mtl->texture_roughness = BMP::parse(path, path);
	else if (split[0] == "map_Nm")
		mtl->texture_metallic = BMP::parse(path, path);
	else if (split[0] == "map_bump" || split[0] == "map_Bump")
		mtl->texture_normal = BMP::parse(path, path);
}

void	parse_number(std::vector<std::string> &split, PBRMaterial *mtl)
{
	float ior;

	if (split[0] == "Np")
		mtl->parallax = std::stof(split[1]);
	else if (split[0] == "Ns")
		mtl->roughness = CLAMP(1.f / (1.f + std::stof(split[1])) * 50.f, 0, 1);
	else if (split[0] == "Nr")
		mtl->roughness = std::stof(split[1]);
	else if (split[0] == "Nm")
		mtl->metallic = std::stof(split[1]);
	else if (split[0] == "Ni")
	{
		ior = std::stof(split[1]);
		ior = (ior - 1) / (ior + 1);
		ior *= ior;
		mtl->specular = new_vec3(ior, ior, ior);
	}
	else if (split[0] == "Tr")
		mtl->alpha = 1 - std::stof(split[1]);
}

void	parse_mtl(t_obj_parser *p, std::string &name)
{
	PBRMaterial	*mtl;
	char		line[4096];

	if (Material::get_by_name(name))
		return ;
	mtl = PBRMaterial::create(name);
	while (fgets(line, 4096, p->fd))
	{
		auto msplit = strsplitwspace(line);
		if (msplit.size() > 1 && msplit[0][0] != '#')
		{
			if (msplit[0][0] == 'K')
				parse_color(msplit, mtl);
			else if (msplit[0][0] == 'N' || msplit[0][0] == 'T')
				parse_number(msplit, mtl);
			else if (msplit[0].find("map_") != std::string::npos)
				parse_texture(p, msplit, mtl);
			else if (msplit[0] == "newmtl")
				parse_mtl(p, msplit[1]);
		}
	}
}

bool			load_mtllib(const std::string &path)
{
	char		line[4096];
	t_obj_parser p;

	auto fullPath = Engine::execution_path() + path;
	if (access(fullPath.c_str(), F_OK | W_OK))
		return (false);
	p.path_split = split_path(fullPath);
	p.fd = fopen(fullPath.c_str(), "r");
	if (!p.fd)
		return (false);
	while (fgets(line, 4096, p.fd))
	{
		auto split = strsplitwspace(line);
		if (split.size() > 1 && split[0][0] != '#')
		{
			if (split[0] == "newmtl")
				parse_mtl(&p, split[1]);
		}
	}
	fclose(p.fd);
	return (true);
}
