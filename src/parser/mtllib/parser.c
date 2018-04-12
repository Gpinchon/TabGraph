/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/12 19:49:22 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parser.h"

void	parse_color(char **split, int mtl)
{
	if (split[0][1] == 'd')
		material_set_albedo(mtl, parse_vec3(&split[1]));
	else if (split[0][1] == 's')
		material_set_specular(mtl, vec3_fdiv(parse_vec3(&split[1]), 
			1 + (1 - material_get_metallic(mtl)) * 24));
	else if (split[0][1] == 'e')
		material_set_emitting(mtl, parse_vec3(&split[1]));
}

void	parse_texture(t_obj_parser *p, char **split, int mtl)
{
	char *path;

	path = ft_strjoin(p->path_split[0], split[1]);
	if (strstr(split[0], "map_Kd"))
		material_set_texture_albedo(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_Ks"))
		material_set_texture_specular(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_Ke"))
		material_set_texture_emitting(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_Nh"))
		material_set_texture_height(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_No"))
		material_set_texture_ao(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_Nr"))
		material_set_texture_roughness(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_Nm"))
		material_set_texture_metallic(mtl, bmp_load(path, path));
	else if (strstr(split[0], "map_bump")
		|| strstr(split[0], "map_Bump"))
		material_set_texture_normal(mtl, bmp_load(path, path));
	free(path);
}

void	parse_number(char **split, int mtl)
{
	if (strstr(split[0], "Np"))
		material_set_parallax(mtl, atof(split[1]));
	else if (strstr(split[0], "Ns"))
		material_set_roughness(mtl, CLAMP(1.f / (1.f + atof(split[1])) * 50.f, 0, 1));
	else if (strstr(split[0], "Nr"))
		material_set_roughness(mtl, atof(split[1]));
	else if (strstr(split[0], "Nm"))
		material_set_metallic(mtl, atof(split[1]));
	else if (strstr(split[0], "Ni"))
	{
		float ior = atof(split[1]);
		ior = (ior - 1) / (ior + 1);
		ior *= ior;
		material_set_specular(mtl, new_vec3(ior, ior, ior));
	}
	else if (strstr(split[0], "Tr"))
		material_set_alpha(mtl, 1 - atof(split[1]));
}

void	parse_mtl(t_obj_parser *p, char **split)
{
	int			mtl;
	char		line[4096];
	char		**msplit;

	if (material_get_index_by_name(split[0]) != -1)
		return ;
	mtl = material_create(split[0]);
	while (fgets(line, 4096, p->fd))
	{
		msplit = ft_strsplitwspace((const char *)line);
		if (msplit[0] && msplit[0][0] != '#')
		{
			if (msplit[0][0] == 'K')
				parse_color(msplit, mtl);
			else if (msplit[0][0] == 'N' || msplit[0][0] == 'T')
				parse_number(msplit, mtl);
			else if (strstr(msplit[0], "map_"))
				parse_texture(p, msplit, mtl);
			else if (!strcmp(msplit[0], "newmtl"))
				parse_mtl(p, &msplit[1]);
		}
		ft_free_chartab(msplit);
	}
}

int	start_mtllib_parsing(t_obj_parser *p, char *path)
{
	char	**split;
	char	line[4096];

	if (access(path, F_OK | W_OK))
		return (-1);
	p->path_split = split_path(path);
	p->fd = fopen(path, "r");
	while (fgets(line, 4096, p->fd))
	{
		split = ft_strsplitwspace((const char *)line);
		if (split && split[0] && split[0][0] != '#')
		{
			if (!strcmp(split[0], "newmtl"))
				parse_mtl(p, &split[1]);
		}
		ft_free_chartab(split);
	}
	ft_free_chartab(p->path_split);
	fclose(p->fd);
	return (0);
}
