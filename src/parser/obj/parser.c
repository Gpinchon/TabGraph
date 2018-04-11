/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/11 20:02:30 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parser.h"

void	parse_f(t_obj_parser *p, char **split)
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

int	start_obj_parsing(t_obj_parser *p, char *path)
{
	char	**split;
	char	line[4096];
	STRING	s;

	if (access(path, F_OK | R_OK) || (p->fd = fopen(path, "r")) <= 0)
		return (-1);
	p->mesh = new_mesh();
	p->vg = new_vgroup();
	p->v = new_ezarray(other, 0, sizeof(VEC3));
	p->vn = new_ezarray(other, 0, sizeof(VEC3));
	p->vt = new_ezarray(other, 0, sizeof(VEC3));
	while (fgets(line, 4096, p->fd))
	{
		split = ft_strsplitwspace((const char *)line);
		if (split && split[0] && split[0][0] != '#')
		{
			if (split[0][0] == 'v')
				parse_vtn(p, split);
			else if (split[0][0] == 'f')
				parse_f(p, &split[1]);
			else if (split[0][0] == 'g'
			|| split[0][0] == 'o'
			|| !strcmp(split[0], "usemtl"))
				parse_vg(p, split);
			else if (!strcmp(split[0], "mtllib"))
			{
				s = new_ezstring(split[1]);
				ezarray_push(&p->mtl_pathes, &s);
			}
			if (!strcmp(split[0], "usemtl"))
				p->vg.mtl_id = hash((unsigned char *)split[1]);
		}
		ft_free_chartab(split);
	}
	if (p->vg.v.length)
		parse_vg(p, split);
	else
		return (-1);
	destroy_ezarray(&p->v);
	destroy_ezarray(&p->vn);
	destroy_ezarray(&p->vt);
	return (0);
}

int	get_mtllib(t_obj_parser *p)
{
	unsigned int i;
	STRING	s;

	i = 0;
	while (i < p->mtl_pathes.length)
	{
		s = *((STRING *)ezarray_get_index(p->mtl_pathes, i));
		char *path = ft_strjoin(p->path_split[0], s.tostring);
		load_mtllib(path);
		free(path);
		destroy_ezstring(&s);
		i++;
	}
	ft_free_chartab(p->path_split);
	destroy_ezarray(&p->mtl_pathes);
	if (material_get_index_by_name("default") == -1)
		material_create("default");
	return (0);
}

void	assign_materials(t_mesh m)
{
	t_vgroup	*vg;
	unsigned	i;

	i = 0;
	while (i < m.vgroups.length)
	{
		vg = (t_vgroup*)ezarray_get_index(m.vgroups, i);
		vg->mtl_index = material_get_index_by_id(vg->mtl_id);
		i++;
	}
}

int	load_obj(char *path)
{
	t_obj_parser	p;

	memset(&p, 0, sizeof(t_obj_parser));
	p.bbox.min = new_vec3(100000, 100000, 100000);
	p.bbox.max = new_vec3(-100000, -100000, -100000);
	p.path_split = split_path(path);
	p.mtl_pathes = new_ezarray(other, 0, sizeof(STRING));
	if (start_obj_parsing(&p, path))
		return (-1);
	get_mtllib(&p);
	if (!p.mesh.vgroups.length && p.vg.v.length)
		ezarray_push(&p.mesh.vgroups, &p.vg);
	p.mesh.bounding_box = p.bbox;
	p.mesh.transform_index = transform_create(new_vec3(0, 0, 0),
		new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	assign_materials(p.mesh);
	ezarray_push(&engine_get()->meshes, &p.mesh);
	return (engine_get()->meshes.length - 1);
}