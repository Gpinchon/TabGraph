/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/15 14:36:49 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/15 18:54:48 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BMP_PARSER_H
# define BMP_PARSER_H

# include <scop.h>
# include <sys/stat.h>

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
	UCHAR				*data;
	unsigned			size_read;
}				t_bmp_parser;

/*
** endif BMP_PARSER_H
*/

#endif
