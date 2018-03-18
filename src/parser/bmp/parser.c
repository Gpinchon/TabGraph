/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/13 19:56:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/18 17:25:04 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static void	convert_bmp(t_bmp_parser *parser)
{
	unsigned char	*pixel_temp;
	unsigned char	rgba[4];
	int				i[3];

	pixel_temp = ft_memalloc(parser->header.size * sizeof(unsigned char));
	i[0] = 0;
	i[1] = -1;
	while (++i[1] < parser->info.width)
	{
		i[2] = -1;
		while (++i[2] < parser->info.height)
		{
			rgba[0] = parser->data[i[0] + 1];
			rgba[1] = parser->data[i[0] + 2];
			rgba[2] = parser->data[i[0] + 3];
			rgba[3] = parser->data[i[0] + 0];
			pixel_temp[i[0] + 0] = rgba[0];
			pixel_temp[i[0] + 1] = rgba[1];
			pixel_temp[i[0] + 2] = rgba[2];
			pixel_temp[i[0] + 3] = rgba[3];
			i[0] += (parser->info.bpp / 8);
		}
	}
	free(parser->data);
	parser->data = pixel_temp;
}

static int	read_data(t_bmp_parser *p, const char *imagepath)
{
	if ((p->fd = open(imagepath, O_RDONLY | O_BINARY)) <= 0)
		return (-1);
	if (read(p->fd, &p->header, sizeof(p->header)) != sizeof(p->header)
	|| read(p->fd, &p->info, sizeof(p->info)) != sizeof(p->info))
	{
		close(p->fd);
		return (-1);
	}
	lseek(p->fd, p->header.data_offset, SEEK_SET);
	p->data = (unsigned char*)ft_memalloc(p->header.size * sizeof(UCHAR));
	p->size_read = read(p->fd, p->data, p->header.size);
	close(p->fd);
	if (p->info.bpp == 32)
		convert_bmp(p);
	return (0);
}

static void	get_format(t_texture *texture)
{
	if (texture->bpp == 8)
	{
		texture->format = GL_RED;
		texture->internal_format = GL_COMPRESSED_RED;
	}
	else if (texture->bpp == 24)
	{
		texture->format = GL_BGR;
		texture->internal_format = GL_COMPRESSED_RGB;
	}
	else if (texture->bpp == 32)
	{
		texture->format = GL_BGRA;
		texture->internal_format = GL_COMPRESSED_RGBA;
	}
}

int			load_bmp(const char *imagepath)
{
	t_bmp_parser	parser;
	t_texture		texture;

	if (access(imagepath, F_OK | R_OK))
		return (-1);
	if (read_data(&parser, imagepath) == -1)
		return (-1);
	ft_memset(&texture, 0, sizeof(t_texture));
	texture.target = GL_TEXTURE_2D;
	texture.name = new_ezstring(imagepath);
	texture.id = hash((unsigned char*)imagepath);
	texture.size.x = parser.info.width;
	texture.size.y = parser.info.height;
	texture.bpp = parser.info.bpp;
	texture.data = parser.data;
	get_format(&texture);
	ezarray_push(&engine_get()->textures, &texture);
	return (engine_get()->textures.length - 1);
}
