/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/19 23:18:56 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>
#include <fcntl.h>

typedef struct		s_obj_parser
{
	char			**path_split;
	int				fd;
	t_engine		*e;
	t_mesh			mesh;
	ARRAY			v;
	ARRAY			vn;
	ARRAY			vt;
	ARRAY			mtl_pathes;
	t_vgroup		vg;
	VEC3			min, max, center;
}					t_obj_parser;

t_vgroup	new_vgroup()
{
	t_vgroup	vg;

	ft_memset(&vg, 0, sizeof(t_vgroup));
	vg.v = new_ezarray(other, 0, sizeof(VEC3));
	vg.vn = new_ezarray(other, 0, sizeof(VEC3));
	vg.vt = new_ezarray(other, 0, sizeof(VEC2));
	vg.tan = new_ezarray(other, 0, sizeof(VEC3));
	vg.bitan = new_ezarray(other, 0, sizeof(VEC3));
	return (vg);
}

t_mesh	new_mesh()
{
	t_mesh			m;

	ft_memset(&m, 0, sizeof(t_mesh));
	m.vgroups = new_ezarray(other, 0, sizeof(t_vgroup));
	return (m);
}

VEC3	parse_vec3(char **split)
{
	float	v[3];
	int		i;

	i = 0;
	ft_memset(v, 0, sizeof(float) * 3);
	while (split[i] && i < 3)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec3(v[0], v[1], v[2]));
}

VEC2	parse_vec2(char **split)
{
	float	v[2];
	int		i;

	i = 0;
	ft_memset(v, 0, sizeof(float) * 3);
	while (split[i] && i < 2)
	{
		v[i] = atof(split[i]);
		i++;
	}
	return (new_vec2(v[0], v[1]));
}

void	parse_vtn(t_obj_parser *p, char **split)
{
	VEC3			v;
	VEC2			vn;

	if (!ft_strcmp(split[0], "v"))
	{
		v = parse_vec3(&split[1]);
		p->min.x = v.x < p->min.x ? v.x : p->min.x;
		p->min.y = v.y < p->min.y ? v.y : p->min.y;
		p->min.z = v.z < p->min.z ? v.z : p->min.z;
		p->max.x = v.x > p->max.x ? v.x : p->max.x;
		p->max.y = v.y > p->max.y ? v.y : p->max.y;
		p->max.z = v.z > p->max.z ? v.z : p->max.z;
		p->center = vec3_fdiv(vec3_add(p->min, p->max), 2);
		if (p->v.length == p->v.reserved)
			ezarray_reserve(&p->v, p->v.length * 2);
		ezarray_push(&p->v, &v);
	}
	else if (!ft_strcmp(split[0], "vn"))
	{
		v = parse_vec3(&split[1]);
		if (p->vn.length == p->vn.reserved)
			ezarray_reserve(&p->vn, p->vn.length * 2);
		ezarray_push(&p->vn, &v);
	}
	else if (!ft_strcmp(split[0], "vt"))
	{
		vn = parse_vec2(&split[1]);
		if (p->vt.length == p->vt.reserved)
			ezarray_reserve(&p->vt, p->vt.length * 2);
		ezarray_push(&p->vt, &vn);
	}
}

void	parse_vg(t_obj_parser *p, char **split)
{
	if (p->vg.v.length > 0)
	{
		ezarray_shrink(&p->vg.v);
		ezarray_shrink(&p->vg.vn);
		ezarray_shrink(&p->vg.vt);
		ezarray_push(&p->mesh.vgroups, &p->vg);
		p->vg = new_vgroup();
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

void	calculate_tan(t_obj_parser *p, VEC3 *v, VEC3 *vn, VEC2 *vt)
{
	int i;

	i = 0;
	VEC3 v0 = v[0];
	VEC3 v1 = v[1];
	VEC3 v2 = v[2];

	// Shortcuts for UVs
	VEC2 uv0 = vt[0];
	VEC2 uv1 = vt[1];
	VEC2 uv2 = vt[2];

	// Edges of the triangle : postion delta
	VEC3 deltaPos1 = vec3_sub(v1, v0);
	VEC3 deltaPos2 = vec3_sub(v2, v0);

	// UV delta
	VEC2 deltaUV1 = vec2_sub(uv1, uv0);
	VEC2 deltaUV2 = vec2_sub(uv2, uv0);
	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	VEC3 tangent = vec3_scale(vec3_sub(vec3_scale(deltaPos1, deltaUV2.y), vec3_scale(deltaPos2, deltaUV1.y)), r);
	//VEC3 bitangent = vec3_scale(vec3_sub(vec3_scale(deltaPos2, deltaUV1.x), vec3_scale(deltaPos1, deltaUV2.x)), r);
	while (i < 3)
	{
		//VEC3	otan = vec3_cross(vn[i], tangent);
		//VEC3	obitan = vec3_cross(vn[i], bitangent);
		VEC3	otan = vec3_normalize(vec3_sub(tangent, vec3_scale(vn[i], vec3_dot(vn[i], tangent))));
		VEC3	obitan = vec3_normalize(vec3_cross(otan, vn[i]));
		ezarray_push(&p->vg.tan, &otan);
		ezarray_push(&p->vg.bitan, &obitan);
		i++;
	}
}

void	parse_v(t_obj_parser *p, char **split, VEC2 *in_vt)
{
	VEC3	v[3];
	VEC3	vn[3];
	VEC2	vt[3];
	int		i;
	int		vindex[3];
	int		tablen;
	int		slash;
	char	**fsplit;

	i = 0;
	while (split[i])
	{
		fsplit = ft_strsplit(split[i], '/');
		vindex[i] = ft_atoi(fsplit[0]);
		if (vindex[i] <= 0 || !ezarray_get_index(p->v, vindex[i] - 1))
			return;
		v[i] = *((VEC3*)ezarray_get_index(p->v, vindex[i] - 1));
		if (!in_vt)
		{
			VEC3 vec = vec3_normalize(vec3_sub(v[i], p->center));
			//vt[i] = new_vec2(vec.x, vec.y);
			float phi = acosf(-vec3_dot(UP, vec));
			vt[i] = new_vec2((phi / M_PI), (acosf(CLAMP(vec3_dot(vec, vec3_cross(UP, (VEC3){0, 0, 1})) / sin(phi), -1, 1)) / (2.f * M_PI)));
		}
		else
			vt[i] = in_vt[i];
		ft_free_chartab(fsplit);
		i++;
	}
	i = 0;
	while (split[i])
	{
		fsplit = ft_strsplit(split[i], '/');
		slash = count_char(split[i], '/');
		tablen = ft_chartablen(fsplit);
		vn[i] = vec3_normalize(vec3_cross(vec3_sub(v[1], v[0]), vec3_sub(v[2], v[0])));
		if (tablen == 3 && slash == 2)
		{
			vt[i] = *((VEC2*)ezarray_get_index(p->vt, ft_atoi(fsplit[1]) - 1));
			vn[i] = *((VEC3*)ezarray_get_index(p->vn, ft_atoi(fsplit[2]) - 1));
		}
		else if (tablen == 2 && slash == 2)
			vn[i] = *((VEC3*)ezarray_get_index(p->vn, ft_atoi(fsplit[1]) - 1));
		else if (tablen == 2 && slash == 1)
			vt[i] = *((VEC2*)ezarray_get_index(p->vt, ft_atoi(fsplit[1]) - 1));
		else
			vn[i] = vec3_normalize(vec3_cross(vec3_sub(v[1], v[0]), vec3_sub(v[2], v[0])));	
		ft_free_chartab(fsplit);
		i++;
	}
	i = 0;
	while (i < 3)
	{
		ezarray_push(&p->vg.v, &v[i]);
		ezarray_push(&p->vg.vt, &vt[i]);
		ezarray_push(&p->vg.vn, &vn[i]);
		i++;
	}
	calculate_tan(p, v, vn, vt);
}

void	parse_f(t_obj_parser *p, char **split)
{
	int		chartablen;

	chartablen = ft_chartablen(split);
	if (chartablen >= 3)
		parse_v(p, (char*[4]){split[0], split[1], split[2], NULL}, chartablen == 4 ? (t_vec2[3]){new_vec2(0, 0), new_vec2(0, 1), new_vec2(1, 1)} : NULL);
	if (chartablen == 4)
		parse_v(p, (char*[4]){split[0], split[2], split[3], NULL}, (t_vec2[3]){new_vec2(0, 0), new_vec2(1, 1), new_vec2(1, 0)});
}

int	start_obj_parsing(t_obj_parser *p, char *path)
{
	char	**split;
	char	*line;
	int		fd;
	STRING	s;

	if (access(path, F_OK | W_OK))
		return (-1);
	p->path_split = split_path(path);
	p->fd = open(path, O_RDONLY);
	p->mesh = new_mesh();
	p->vg = new_vgroup();
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
			else if (split[0][0] == 'g'
			|| split[0][0] == 'o'
			|| !ft_strcmp(split[0], "usemtl"))
				parse_vg(p, split);
			else if (!ft_strcmp(split[0], "mtllib"))
			{
				s = new_ezstring(split[1]);
				ezarray_push(&p->mtl_pathes, &s);
			}
			if (!ft_strcmp(split[0], "usemtl"))
				p->vg.mtl_id = hash((unsigned char *)split[1]);
		}
		ft_free_chartab(split);
		free(line);
	}
	ezarray_push(&p->mesh.vgroups, &p->vg);
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
		printf("mtllib : %s\n", path);
		load_mtllib(p->e, path);
		free(path);
		destroy_ezstring(&s);
		i++;
	}
	ft_free_chartab(p->path_split);
	destroy_ezarray(&p->mtl_pathes);
	return (0);
}

void	assign_materials(t_engine *e, t_mesh m)
{
	t_vgroup	*vg;
	unsigned	i;

	i = 0;
	while (i < m.vgroups.length)
	{
		vg = (t_vgroup*)ezarray_get_index(m.vgroups, i);
		vg->mtl_index = get_material_index_by_id(e->materials, vg->mtl_id);
		i++;
	}
}

int	load_obj(t_engine *engine, char *path)
{
	t_obj_parser	p;

	ft_memset(&p, 0, sizeof(t_obj_parser));
	p.e = engine;
	p.path_split = split_path(path);
	p.mtl_pathes = new_ezarray(other, 0, sizeof(STRING));
	if (start_obj_parsing(&p, path))
		return (-1);
	get_mtllib(&p);
	if (!p.mesh.vgroups.length && p.vg.v.length)
		ezarray_push(&p.mesh.vgroups, &p.vg);
	p.mesh.transform_index = create_transform(engine, new_vec3(0, 0, 0),
		new_vec3(0, 0, 0), new_vec3(1, 1, 1));
	assign_materials(engine, p.mesh);
	ezarray_push(&engine->meshes, &p.mesh);
	return (engine->meshes.length);
}