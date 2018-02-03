/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/03 14:34:29 by gpinchon         ###   ########.fr       */
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
	if (split[0][5] == 'd')
	{
		path = ft_strjoin(p->path_split[0], split[1]);
		mtl->data.texture_albedo = load_bmp(p->e, path);
		free(path);
	}
	else if (split[0][5] == 'h')
	{
		path = ft_strjoin(p->path_split[0], split[1]);
		mtl->data.texture_height = load_bmp(p->e, path);
		free(path);
	}
	else if (split[0][5] == 'r')
	{
		path = ft_strjoin(p->path_split[0], split[1]);
		mtl->data.texture_roughness = load_bmp(p->e, path);
		free(path);
	}
	else if (split[0][5] == 'm')
	{
		path = ft_strjoin(p->path_split[0], split[1]);
		mtl->data.texture_metallic = load_bmp(p->e, path);
		free(path);
	}
	else if (ft_strstr(&split[0][4], "bump\0"))
	{
		path = ft_strjoin(p->path_split[0], split[1]);
		mtl->data.texture_normal = load_bmp(p->e, path);
		free(path);
	}
}

void	parse_mtl(t_mtl_parser *p, char **split)
{
	t_material	mtl;
	int			fd;
	char		*line;
	char		**msplit;

	mtl = new_material();
	mtl.name = new_ezstring(split[0]);
	mtl.id = hash((unsigned char *)mtl.name.tostring);
	fd = p->fd;
	while (get_next_line(fd, &line))
	{
		msplit = ft_strsplitwspace((const char *)line);
		if (!ft_strchr(msplit[0], '#'))
		{
			if (msplit[0] && msplit[0][0] == 'K')
				parse_color(p, &msplit[0], &mtl);
			else if (msplit[0] && ft_strstr(msplit[0], "map"))
				parse_texture(p, msplit, &mtl);
			else if (msplit[0] && !ft_strcmp(msplit[0], "Tr"))
				mtl.data.alpha = 1 - atof(msplit[1]);
			else if (msplit[0] && !ft_strcmp(msplit[0], "Np"))
				mtl.data.parallax = atof(msplit[1]);
			else if (msplit[0] && !ft_strcmp(msplit[0], "Ns"))
				mtl.data.roughness = CLAMP(1.f / (1.f + atof(msplit[1])) * 50.f, 0, 1);
			else if (msplit[0] && !ft_strcmp(msplit[0], "Nr"))
				mtl.data.roughness = atof(msplit[1]);
			else if (msplit[0] && !ft_strcmp(msplit[0], "Nm"))
				mtl.data.metallic = atof(msplit[1]);
			else if (msplit[0] && !ft_strcmp(msplit[0], "Ni"))
			{
				mtl.data.refraction = atof(msplit[1]);
				printf("Ni %f\n", mtl.data.refraction);
			}
			else if (msplit[0] && !ft_strcmp(msplit[0], "newmtl"))
				parse_mtl(p, &msplit[1]);
		}
		ft_free_chartab(msplit);
		free(line);
	}
	if (!get_material_index_by_name(p->e->materials, mtl.name.tostring))
		ezarray_push(&p->e->materials, &mtl);
	else
		destroy_ezstring(&mtl.name);
}

int	start_mtllib_parsing(t_mtl_parser *p, char *path)
{
	char	**split;
	char	*line;
	int		fd;

	if (access(path, F_OK | W_OK))
		return (-1);
	p->path_split = split_path(path);
	p->fd = open(path, O_RDONLY);
	fd = p->fd;
	while (get_next_line(fd, &line))
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

	ft_memset(&p, 0, sizeof(t_mtl_parser));
	p.e = engine;
	if (start_mtllib_parsing(&p, path))
		return (-1);
	/*int	i;
	t_material	*mtl;

	i = 0;
	while ((mtl = ezarray_get_index(p.e->materials, i)))
	{
		printf("load_mtllib -> mtl id %I64d\n", mtl->id);
		i++;
	}*/
	return (engine->materials.length);
}