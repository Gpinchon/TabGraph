/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/08 01:51:55 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <fcntl.h>

typedef struct		s_mtl_parser
{
	char			**path_split;
	int				fd;
}					t_mtl_parser;

void	parse_color(t_mtl_parser *p, char **split, int mtl)
{
	if (split[0][1] == 'd')
		material_set_albedo(mtl, parse_vec3(&split[1]));
	else if (split[0][1] == 's')
		material_set_specular(mtl, parse_vec3(&split[1]));
	else if (split[0][1] == 'e')
		material_set_emitting(mtl, parse_vec3(&split[1]));
	(void)p;
}

void	parse_texture(t_mtl_parser *p, char **split, int mtl)
{
	char *path;

	path = ft_strjoin(p->path_split[0], split[1]);
	if (ft_strstr(split[0], "map_Kd"))
		material_set_texture_albedo(mtl, load_bmp(path));
	else if (ft_strstr(split[0], "map_Ks"))
		material_set_texture_specular(mtl, load_bmp(path));
	else if (ft_strstr(split[0], "map_Ke"))
	{
		printf("%s\n", path);
		material_set_texture_emitting(mtl, load_bmp(path));
		printf("%i\n", material_get_texture_emitting(mtl));
	}
	else if (ft_strstr(split[0], "map_Nh"))
		material_set_texture_height(mtl, load_bmp(path));
	else if (ft_strstr(split[0], "map_No"))
		material_set_texture_ao(mtl, load_bmp(path));
	else if (ft_strstr(split[0], "map_Nr"))
		material_set_texture_roughness(mtl, load_bmp(path));
	else if (ft_strstr(split[0], "map_Nm"))
		material_set_texture_metallic(mtl, load_bmp(path));
	else if (ft_strstr(split[0], "map_bump")
		|| ft_strstr(split[0], "map_Bump"))
		material_set_texture_normal(mtl, load_bmp(path));
	free(path);
}

void	parse_number(t_mtl_parser *p, char **split, int mtl)
{
	if (ft_strstr(split[0], "Np"))
		material_set_parallax(mtl, atof(split[1]));
	else if (ft_strstr(split[0], "Ns"))
		material_set_roughness(mtl, CLAMP(1.f / (1.f + atof(split[1])) * 50.f, 0, 1));
	else if (ft_strstr(split[0], "Nr"))
		material_set_roughness(mtl, atof(split[1]));
	else if (ft_strstr(split[0], "Nm"))
		material_set_metallic(mtl, atof(split[1]));
	else if (ft_strstr(split[0], "Ni"))
	{
		float ior = atof(split[1]);
		ior = (ior - 1) / (ior + 1);
		ior *= ior;
		material_set_specular(mtl, new_vec3(ior, ior, ior));
	}
	else if (ft_strstr(split[0], "Tr"))
		material_set_alpha(mtl, 1 - atof(split[1]));
	(void)p;
}

void	parse_mtl(t_mtl_parser *p, char **split)
{
	int			mtl;
	char		*line;
	char		**msplit;

	if (material_get_index_by_name(split[0]) != -1)
		return ;
	mtl = material_create(split[0]);
	while (get_next_line(p->fd, &line) == 1)
	{
		msplit = ft_strsplitwspace((const char *)line);
		if (msplit[0] && msplit[0][0] != '#')
		{
			if (msplit[0][0] == 'K')
				parse_color(p, msplit, mtl);
			else if (msplit[0][0] == 'N' || msplit[0][0] == 'T')
				parse_number(p, msplit, mtl);
			else if (ft_strstr(msplit[0], "map_"))
				parse_texture(p, msplit, mtl);
			else if (!ft_strcmp(msplit[0], "newmtl"))
				parse_mtl(p, &msplit[1]);
		}
		ft_free_chartab(msplit);
		free(line);
	}
	//material_assign_shader(engine_get()->materials.length - 1, shader_get_by_name("default"));
}

int	start_mtllib_parsing(t_mtl_parser *p, char *path)
{
	char	**split;
	char	*line;

	if (access(path, F_OK | W_OK))
		return (-1);
	p->path_split = split_path(path);
	p->fd = open(path, O_RDONLY);
	while (get_next_line(p->fd, &line))
	{
		split = ft_strsplitwspace((const char *)line);
		if (split && split[0] && split[0][0] != '#')
		{
			if (!ft_strcmp(split[0], "newmtl"))
				parse_mtl(p, &split[1]);
		}
		ft_free_chartab(split);
		free(line);
	}
	ft_free_chartab(p->path_split);
	close(p->fd);
	return (0);
}

int	load_mtllib(char *path)
{
	t_mtl_parser	p;

	ft_memset(&p, 0, sizeof(t_mtl_parser));
	if (start_mtllib_parsing(&p, path))
		return (-1);
	return (engine_get()->materials.length);
}