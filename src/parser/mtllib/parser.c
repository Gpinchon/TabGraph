/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anonymous <anonymous@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/26 11:57:18 by anonymous        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <fcntl.h>

typedef struct		s_mtl_parser
{
	char			**path_split;
	int				fd;
}					t_mtl_parser;

void	parse_color(t_mtl_parser *p, char **split, t_material *mtl)
{
	if (split[0][1] == 'd')
		mtl->data.albedo = parse_vec3(&split[1]);
	if (split[0][1] == 'e')
		mtl->data.emitting = parse_vec3(&split[1]);
	(void)p;
}

void	parse_texture(t_mtl_parser *p, char **split, t_material *mtl)
{
	char *path;

	path = ft_strjoin(p->path_split[0], split[1]);
	if (ft_strstr(split[0], "map_Kd"))
		mtl->data.texture_albedo = load_bmp(path);
	else if (ft_strstr(split[0], "map_Ke"))
		mtl->data.texture_emitting = load_bmp(path);
	else if (ft_strstr(split[0], "map_Nh"))
		mtl->data.texture_height = load_bmp(path);
	else if (ft_strstr(split[0], "map_Nr"))
		mtl->data.texture_roughness = load_bmp(path);
	else if (ft_strstr(split[0], "map_Nm"))
		mtl->data.texture_metallic = load_bmp(path);
	else if (ft_strstr(split[0], "map_bump")
		|| ft_strstr(split[0], "map_Bump"))
		mtl->data.texture_normal = load_bmp(path);
	free(path);
}

void	parse_number(t_mtl_parser *p, char **split, t_material *mtl)
{
	if (ft_strstr(split[0], "Np"))
		mtl->data.parallax = atof(split[1]);
	else if (ft_strstr(split[0], "Ns"))
		mtl->data.roughness = CLAMP(1.f / (1.f + atof(split[1])) * 50.f, 0, 1);
	else if (ft_strstr(split[0], "Nr"))
		mtl->data.roughness = atof(split[1]);
	else if (ft_strstr(split[0], "Nm"))
		mtl->data.metallic = atof(split[1]);
	else if (ft_strstr(split[0], "Ni"))
		mtl->data.refraction = atof(split[1]);
	else if (ft_strstr(split[0], "Tr"))
		mtl->data.alpha = 1 - atof(split[1]);
	(void)p;
}

void	parse_mtl(t_mtl_parser *p, char **split)
{
	t_material	mtl;
	char		*line;
	char		**msplit;

	if (material_get_index_by_name(split[0]) != -1)
		return ;
	mtl = new_material(split[0]);
	mtl.data.texture_stupid = texture_get_by_name("./res/stupid.bmp");
	while (get_next_line(p->fd, &line) == 1)
	{
		msplit = ft_strsplitwspace((const char *)line);
		if (msplit[0] && msplit[0][0] != '#')
		{
			if (msplit[0][0] == 'K')
				parse_color(p, msplit, &mtl);
			else if (msplit[0][0] == 'N' || msplit[0][0] == 'T')
				parse_number(p, msplit, &mtl);
			else if (ft_strstr(msplit[0], "map_"))
				parse_texture(p, msplit, &mtl);
			else if (!ft_strcmp(msplit[0], "newmtl"))
				parse_mtl(p, &msplit[1]);
		}
		ft_free_chartab(msplit);
		free(line);
	}
	ezarray_push(&engine_get()->materials, &mtl);
	material_assign_shader(engine_get()->materials.length - 1, shader_get_by_name("default"));
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