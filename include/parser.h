/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/10 18:41:58 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/01 10:05:43 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H
# include "scop.hpp"
# include <fcntl.h>

typedef struct					s_obj_parser
{
	std::vector<std::string>	path_split;
	FILE						*fd;
	std::vector<VEC3>			v;
	std::vector<VEC3>			vn;
	std::vector<VEC2>			vt;
	Mesh						*parent;
	Mesh						*vg;
	AABB						bbox;
}								t_obj_parser;

VEC3			parse_vec3(std::vector<std::string> &split);
VEC2			parse_vec2(std::vector<std::string> &split);
void			parse_vg(t_obj_parser *p);
void			parse_v(t_obj_parser *p, std::vector<std::string> &split, VEC2 *in_vt);
void			parse_vtn(t_obj_parser *p, std::vector<std::string> &split);
void			correct_vt(VEC2 *vt);
VEC2			generate_vt(VEC3 v, VEC3 center);
VEC3			generate_vn(VEC3 *v);
std::string		strjoin(char const *s1, char const *s2);
std::vector<std::string>	split_path(const std::string &path);
std::vector<std::string>	strsplit(const std::string &s, char c);
std::vector<std::string>	strsplitwspace(const std::string &s);
unsigned		count_char(const std::string &str, char c);
std::string		stream_to_str(FILE *stream);

//std::map<std::string, ShaderVariable>			get_shader_uniforms(Shader *shader);
//std::map<std::string, ShaderVariable>			get_shader_attributes(Shader *shader);

#endif
