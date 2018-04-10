/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:41:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/10 18:55:29 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H
# include <scope.h>
# include <fcntl.h>

typedef struct		s_obj_parser
{
	char			**path_split;
	int				fd;
	t_mesh			mesh;
	ARRAY			v;
	ARRAY			vn;
	ARRAY			vt;
	ARRAY			mtl_pathes;
	t_vgroup		vg;
	t_aabb			bbox;
}					t_obj_parser;

t_vgroup	new_vgroup();
t_mesh		new_mesh();
VEC3		parse_vec3(char **split);
VEC2		parse_vec2(char **split);
void		parse_vg(t_obj_parser *p, char **split);
void		parse_v(t_obj_parser *p, char **split, VEC2 *in_vt);
void		parse_vtn(t_obj_parser *p, char **split);

#endif