/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:41:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/15 16:44:30 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H
# include <scop.h>
# include <fcntl.h>

typedef struct		s_obj_parser
{
	char			**path_split;
	FILE			*fd;
	t_mesh			mesh;
	ARRAY			v;
	ARRAY			vn;
	ARRAY			vt;
	t_vgroup		vg;
	t_aabb			bbox;
}					t_obj_parser;

t_vgroup	new_vgroup();
t_mesh		new_mesh();
VEC3		parse_vec3(char **split);
VEC2		parse_vec2(char **split);
void		parse_vg(t_obj_parser *p);
void		parse_v(t_obj_parser *p, char **split, VEC2 *in_vt);
void		parse_vtn(t_obj_parser *p, char **split);
void		correct_vt(VEC2 *vt);
VEC2		generate_vt(VEC3 v, VEC3 center);
VEC3		generate_vn(VEC3 *v);
int			start_mtllib_parsing(t_obj_parser *p, char *path);
ARRAY		get_shader_uniforms(t_shader *shader);
ARRAY		get_shader_attributes(t_shader *shader);

#endif