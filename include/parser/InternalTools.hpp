/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InternalTools.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:16:45 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/13 00:16:45 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Mesh.hpp"
#include "Texture.hpp"
#include <vector>
#include <string>
# include <fcntl.h>

# ifndef O_BINARY
#  define O_BINARY 0x0004
# endif

typedef struct					s_obj_parser
{
	s_obj_parser() : fd(nullptr), parent(nullptr), vg(nullptr) {};
	std::vector<std::string>	path_split;
	FILE						*fd;
	std::vector<VEC3>			v;
	std::vector<VEC3>			vn;
	std::vector<VEC2>			vt;
	Mesh						*parent;
	Mesh						*vg;
	AABB						bbox;
}								t_obj_parser;

# pragma pack(1)
typedef struct	s_bmp_header
{
	short		type;
	unsigned	size;
	short		reserved1;
	short		reserved2;
	unsigned	data_offset;
}				t_bmp_header;

typedef struct	s_bmp_info
{
	unsigned	header_size;
	int			width;
	int			height;
	short		color_planes;
	short		bpp;
	unsigned	compression_method;
	unsigned	size;
	int			horizontal_resolution;
	int			vertical_resolution;
	int			colors;
	int			important_colors;
}				t_bmp_info;
# pragma pack()

typedef struct	s_bmp_parser
{
	int					fd;
	t_bmp_info			info;
	t_bmp_header		header;
	unsigned char		*data;
	unsigned			size_read;
}				t_bmp_parser;


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
std::string		convert_backslash(std::string str);
