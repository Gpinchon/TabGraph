/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2016/08/11 12:32:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <fcntl.h>

typedef struct		s_obj_parser
{
	char			**path_split;
	int				fd;
	t_mesh			mesh;
	ARRAY			v;
	ARRAY			vn;
	ARRAY			vt;
	t_vert_group	vg;
}					t_obj_parser;

t_vert_group	new_vert_group()
{
	t_vert_group	vg;

	ft_memset(&vg, 0, sizeof(t_vert_group));
	vg.v = new_ezarray(other, 0, sizeof(VEC3));
	vg.vn = new_ezarray(other, 0, sizeof(VEC3));
	vg.vt = new_ezarray(other, 0, sizeof(VEC3));
	vg.vindex = new_ezarray(other, 0, sizeof(int));
	return (vg);
}

t_mesh	new_mesh()
{
	t_mesh			m;

	ft_memset(&m, 0, sizeof(t_mesh));
	m.vert_groups = new_ezarray(other, 0, sizeof(t_vert_group));
	return (m);
}

VEC3	parse_vec3(char **split)
{
	float	v[3];
	int		i;

	i = 0;
	ft_memset(v, 0, sizeof(float));
	while (split[i] && i < 3)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec3(v[0], v[1], v[2]));
}

void	parse_vtn(t_obj_parser *p, char **split)
{
	VEC3			v;

	v = parse_vec3(&split[1]);
	if (!ft_strcmp(split[0], "v"))
		ezarray_push(&p->v, &v);
	else if (!ft_strcmp(split[0], "vn"))
		ezarray_push(&p->vn, &v);
	else if (!ft_strcmp(split[0], "vt"))
		ezarray_push(&p->vt, &v);
}

void	parse_vg(t_obj_parser *p, char **split)
{
	if (p->vg.v.length > 0)
	{
		ezarray_push(&p->mesh.vert_groups, &p->vg);
		p->vg = new_vert_group();
	}
	(void)split;
}

int		ft_chartablen(char **s)
{
	int	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

char	vindex_in_array(ARRAY a, int index)
{
	int	i;
	int	*value;

	i = 0;
	while ((value = ezarray_get_index(a, i)))
	{
		if (*value == index)
			return (1);
		i++;
	}
	return (0);
}

void	parse_v(t_obj_parser *p, char **split)
{
	VEC3	v[3];
	int		i;
	int		vindex;
	char	**fsplit;

	i = 0;
	while (split[i] && i < 3)
	{
		fsplit = ft_strsplit(split[i], '/');
		vindex = ft_atoi(fsplit[0]);
		v[i] = *((VEC3*)ezarray_get_index(p->v, vindex - 1));
		if (ft_chartablen(fsplit) == 1)
		{
			if (!vindex_in_array(p->vg.vindex, vindex))
				ezarray_push(&p->vg.v, &v[i]);
		}
		else
		{
			ezarray_push(&p->vg.v, &v[i]);
			vindex = p->vg.v.length;
		}
		ezarray_push(&p->vg.vindex, &vindex);
		ft_free_chartab(fsplit);
		i++;
	}
}

void	parse_f(t_obj_parser *p, char **split)
{
	int		chartablen;
	char	*tri[3];

	chartablen = ft_chartablen(split);
	if (chartablen == 3)
		parse_v(p, split);
	else if (chartablen == 4)
	{
		tri[0] = split[0];
		tri[1] = split[2];
		tri[2] = split[3];
		parse_v(p, split);
		parse_v(p, tri);
	}
}

int	start_parsing(t_obj_parser *p, char *path)
{
	char	**split;
	char	*line;
	int		fd;

	if (access(path, F_OK | W_OK))
		return (-1);
	ft_memset(p, 0, sizeof(t_obj_parser));
	p->path_split = split_path(path);
	p->fd = open(path, O_RDONLY);
	p->mesh = new_mesh();
	p->vg = new_vert_group();
	p->v = new_ezarray(other, 0, sizeof(VEC3));
	p->vn = new_ezarray(other, 0, sizeof(VEC3));
	p->vt = new_ezarray(other, 0, sizeof(VEC3));
	fd = p->fd;
	while (get_next_line(fd, &line))
	{
		split = ft_strsplitwspace((const char *)line);
		if (split && split[0] && split[0][0] != '#')
		{
			if (split[0][0] == 'v')
				parse_vtn(p, split);
			else if (split[0][0] == 'f')
				parse_f(p, &split[1]);
			else if (split[0][0] == 'g')
				parse_vg(p, split);
		}
		ft_free_chartab(split);
		free(line);
	}
	ft_free_chartab(p->path_split);
	destroy_ezarray(&p->v);
	destroy_ezarray(&p->vn);
	destroy_ezarray(&p->vt);
	return (0);
}

int	load_obj(t_engine *engine, char *path)
{
	t_obj_parser	p;

	if (start_parsing(&p, path))
		return (-1);
	if (!p.mesh.vert_groups.length && p.vg.v.length)
		ezarray_push(&p.mesh.vert_groups, &p.vg);
	ezarray_push(&engine->meshes, &p.mesh);
	VEC3	*v;
	int		i;
	int		j;
	int		*vindex;
	t_vert_group	*vg;
	i = 0;
	j = 0;
	while ((vg = ezarray_get_index(p.mesh.vert_groups, i)))
	{
		j = 0;
		while ((vindex = ezarray_get_index(vg->vindex, j)))
		{
			v = ezarray_get_index(vg->v, *vindex - 1);
			printf("v %i : %f, %f, %f\n", *vindex, v->x, v->y, v->z);
			if (!((j + 1)%3))
				printf("\n");
			j++;
		}
		i++;
	}
	return (engine->meshes.length);
}