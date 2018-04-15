/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 14:45:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <parser.h>

static void	parse_f(t_obj_parser *p, char **split)
{
	short	faces;
	short	i;

	faces = ft_chartablen(split) - 3 + 1;
	i = 0;
	while (i < faces)
	{
		if (faces == 2 && i == 0)
			parse_v(p, (char*[4]){split[0], split[i + 1], split[i + 2], NULL},
				(t_vec2[3]){new_vec2(0, 0), new_vec2(0, 1), new_vec2(1, 1)});
		else if (faces == 2 && i >= 1)
			parse_v(p, (char*[4]){split[0], split[i + 1], split[i + 2], NULL},
				(t_vec2[3]){new_vec2(0, 0), new_vec2(1, 1), new_vec2(1, 0)});
		else
			parse_v(p, (char*[4]){split[0], split[i + 1], split[i + 2], NULL},
				NULL);
		i++;
	}
}

static void	parse_line(t_obj_parser *p, const char *line)
{
	char	**split;
	char	*path;

	split = ft_strsplitwspace((const char *)line);
	if (!split || !split[0] || split[0][0] == '#')
		return ;
	if (split[0][0] == 'v')
		parse_vtn(p, split);
	else if (split[0][0] == 'f')
		parse_f(p, &split[1]);
	else if (split[0][0] == 'g' || split[0][0] == 'o'
		|| !strcmp(split[0], "usemtl"))
		parse_vg(p);
	else if (!strcmp(split[0], "mtllib"))
	{
		path = ft_strjoin(p->path_split[0], split[1]);
		load_mtllib(path);
		free(path);
	}
	if (!strcmp(split[0], "usemtl"))
	{
		p->vg.mtl_id = hash((unsigned char *)split[1]);
		p->vg.mtl_index = material_get_index_by_id(p->vg.mtl_id);
	}
	ft_free_chartab(split);
}

static int	start_obj_parsing(t_obj_parser *p, char *path)
{
	char	line[4096];

	if (access(path, F_OK | R_OK) || !(p->fd = fopen(path, "r")))
		return (-1);
	p->mesh = new_mesh();
	p->vg = new_vgroup();
	p->v = new_ezarray(other, 0, sizeof(VEC3));
	p->vn = new_ezarray(other, 0, sizeof(VEC3));
	p->vt = new_ezarray(other, 0, sizeof(VEC2));
	while (fgets(line, 4096, p->fd))
		parse_line(p, line);
	if (p->vg.v.length)
		parse_vg(p);
	else
		return (-1);
	destroy_ezarray(&p->v);
	destroy_ezarray(&p->vn);
	destroy_ezarray(&p->vt);
	return (0);
}

int			load_obj(char *path)
{
	t_obj_parser	p;

	memset(&p, 0, sizeof(t_obj_parser));
	p.bbox.min = new_vec3(100000, 100000, 100000);
	p.bbox.max = new_vec3(-100000, -100000, -100000);
	p.path_split = split_path(path);
	if (start_obj_parsing(&p, path))
		return (-1);
	if (material_get_index_by_name("default") == -1)
		material_create("default");
	if (!p.mesh.vgroups.length && p.vg.v.length)
		ezarray_push(&p.mesh.vgroups, &p.vg);
	p.mesh.bounding_box = p.bbox;
	p.mesh.transform_index = transform_create(new_vec3(0, 0, 0),
		new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	ezarray_push(&engine_get()->meshes, &p.mesh);
	return (engine_get()->meshes.length - 1);
}