/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/13 19:56:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/15 14:38:03 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

void			convert_bmp(t_bmp_parser *bmp_parser, t_bmp_info *bmp_info)
{
	UCHAR	*pixel_temp = (UCHAR*)calloc(bmp_parser->header.size, sizeof(UCHAR));
	UCHAR	a, b, g, r;
	int		i = 0, x, y;

	for (y = 0; y < bmp_info->height; y++)
	{
		for(x = 0; x < bmp_info->width; x++)
		{
			r = bmp_parser->data[i + 1];
			g = bmp_parser->data[i + 2];
			b = bmp_parser->data[i + 3];
			pixel_temp[i + 0] = r;
			pixel_temp[i + 1] = g;
			pixel_temp[i + 2] = b;
			if (bmp_info->bpp == 32)
			{
				a = bmp_parser->data[i];
				pixel_temp[i + 3] = a;
			}
			i += (bmp_info->bpp / 8);
		}
	}
    free(bmp_parser->data);
    bmp_parser->data = pixel_temp;
}

int			load_bmp(const char *imagepath)
{
	t_bmp_parser	parser;
	t_bmp_info		bmp_info;
	t_texture		texture;

	if (access(imagepath, F_OK | R_OK))
		return (-1);
	if ((parser.fd = open(imagepath, O_RDONLY | O_BINARY)) <= 0)
		return(-1);
	if (read(parser.fd, &parser.header, sizeof(t_bmp_header)) != sizeof(t_bmp_header)
	|| read(parser.fd, &bmp_info, sizeof(bmp_info)) != sizeof(bmp_info))
	{
		close(parser.fd);
		return(-1);
	}
	lseek(parser.fd, parser.header.data_offset, SEEK_SET);
	parser.data = (UCHAR*)ft_memalloc(bmp_info.size * sizeof(UCHAR));
	parser.size_read = read(parser.fd, parser.data, bmp_info.size);
	close(parser.fd);
	if (bmp_info.bpp == 32)
		convert_bmp(&parser, &bmp_info);
	ft_memset(&texture, 0, sizeof(t_texture));
	texture.target = GL_TEXTURE_2D;
	texture.name = new_ezstring(imagepath);
	texture.id = hash((unsigned char*)imagepath);
	texture.size.x = bmp_info.width;
	texture.size.y = bmp_info.height;
	texture.bpp = bmp_info.bpp;
	texture.data = parser.data;
	ezarray_push(&engine_get()->textures, &texture);
	return (engine_get()->textures.length - 1);
}