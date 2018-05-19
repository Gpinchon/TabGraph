/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   InternalTools.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 00:16:45 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 23:05:28 by gpinchon         ###   ########.fr       */
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

struct	t_obj_parser
{
	std::vector<std::string>	path_split;
	FILE						*fd{nullptr};
	std::vector<VEC3>			v;
	std::vector<VEC3>			vn;
	std::vector<VEC2>			vt;
	Mesh						*parent{nullptr};
	Mesh						*vg{nullptr};
	AABB						bbox;
};

# pragma pack(1)
struct	t_bmp_header
{
	short		type;
	unsigned	size;
	short		reserved1;
	short		reserved2;
	unsigned	data_offset;
};

struct	t_bmp_info
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
};
# pragma pack()

struct	t_bmp_parser
{
	int					fd;
	t_bmp_info			info;
	t_bmp_header		header;
	GLubyte				*data{nullptr};
	unsigned			size_read;
};


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
