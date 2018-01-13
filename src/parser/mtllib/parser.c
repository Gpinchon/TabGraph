/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/20 18:20:52 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/12 22:50:09 by gpinchon         ###   ########.fr       */
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
	/*if (split[0][1] == 'a')
		mtl->data.bpr.ambient = parse_vec3(&split[1]);
	else */
	if (split[0][1] == 'd')
		mtl->data.pbr.albedo = parse_vec3(&split[1]);
	/*else if (split[0][1] == 's')
		mtl->data.blin.specular = parse_vec3(&split[1]);*/
	(void)p;
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
		if (msplit[0] && msplit[0][0] == 'K')
			parse_color(p, &msplit[0], &mtl);
		else if (msplit[0] && !ft_strcmp(msplit[0], "Ns"))
			mtl.data.pbr.specular = atof(msplit[1]);
		else if (msplit[0] && !ft_strcmp(msplit[0], "newmtl"))
			parse_mtl(p, &msplit[1]);
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
	int	i;
	t_material	*mtl;

	i = 0;
	while ((mtl = ezarray_get_index(p.e->materials, i)))
	{
		printf("load_mtllib -> mtl id %I64d\n", mtl->id);
		i++;
	}
	return (engine->materials.length);
}