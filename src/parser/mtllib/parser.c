/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/08 00:37:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <fcntl.h>

typedef struct		s_mtl_parser
{
	char			**path_split;
	int				fd;
	t_engine		*e;
}					t_mtl_parser;

t_material	new_material(char *name)
{
	t_material	mtl;

	ft_memset(&mtl, 0, sizeof(t_material));
	ft_memset(&mtl.data, -1, sizeof(t_mtl));
	mtl.data.alpha = 1;
	mtl.data.parallax = 0.01;
	mtl.data.albedo = new_vec3(0, 0, 0);
	mtl.data.emitting = new_vec3(0, 0, 0);
	mtl.data.metallic = 0;
	mtl.data.roughness = 0.5;
	mtl.data.refraction = 1.5;
	mtl.name = new_ezstring(name);
	mtl.id = hash((unsigned char *)mtl.name.tostring);
	return (mtl);
}

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
		mtl->data.texture_albedo = load_bmp(p->e, path);
	else if (ft_strstr(split[0], "map_Ke"))
		mtl->data.texture_emitting = load_bmp(p->e, path);
	else if (ft_strstr(split[0], "map_Kh"))
		mtl->data.texture_height = load_bmp(p->e, path);
	else if (ft_strstr(split[0], "map_Kr"))
		mtl->data.texture_roughness = load_bmp(p->e, path);
	else if (ft_strstr(split[0], "map_Km"))
		mtl->data.texture_metallic = load_bmp(p->e, path);
	else if (ft_strstr(split[0], "map_bump"))
		mtl->data.texture_normal = load_bmp(p->e, path);
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

	if (material_get_index_by_name(p->e, split[0]) != -1)
		return ;
	mtl = new_material(split[0]);
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
	ezarray_push(&p->e->materials, &mtl);
	material_assign_shader(p->e, p->e->materials.length - 1, shader_get_by_name(p->e, "default"));
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

int	load_mtllib(t_engine *engine, char *path)
{
	t_mtl_parser	p;
	t_material		mtl;

	ft_memset(&p, 0, sizeof(t_mtl_parser));
	p.e = engine;
	if (material_get_index_by_name(engine, "default") == -1)
	{
		mtl = new_material("default");
		ezarray_push(&engine->materials, &mtl);
		material_assign_shader(engine, engine->materials.length - 1, shader_get_by_name(engine, "default"));
	}
	if (start_mtllib_parsing(&p, path))
		return (-1);
	return (engine->materials.length);
}