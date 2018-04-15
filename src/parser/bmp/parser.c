/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/13 19:56:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/15 18:55:14 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <bmp_parser.h>

static void	convert_bmp(t_bmp_parser *parser)
{
	unsigned char	*pixel_temp;
	unsigned char	rgba[4];
	int				i[3];

	pixel_temp = calloc(parser->header.size, sizeof(unsigned char));
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
	unsigned	data_size;

	if ((p->fd = open(imagepath, O_RDONLY | O_BINARY)) <= 0)
		return (-1);
	if (read(p->fd, &p->header, sizeof(p->header)) != sizeof(p->header)
	|| read(p->fd, &p->info, sizeof(p->info)) != sizeof(p->info)
	|| p->header.type != 0x4D42)
	{
		close(p->fd);
		return (-1);
	}
	data_size = p->info.bpp / 8 * p->info.width * p->info.height;
	lseek(p->fd, p->header.data_offset, SEEK_SET);
	p->data = (unsigned char*)calloc(1, data_size);
	p->size_read = read(p->fd, p->data, data_size);
	close(p->fd);
	if (p->info.bpp == 32)
		convert_bmp(p);
	return (0);
}

static void	get_format(UCHAR bpp, GLenum *format, GLenum *internal_format)
{
	if (bpp == 8)
	{
		*format = GL_RED;
		*internal_format = GL_COMPRESSED_RED;
	}
	else if (bpp == 24)
	{
		*format = GL_BGR;
		*internal_format = GL_COMPRESSED_RGB;
	}
	else if (bpp == 32)
	{
		*format = GL_BGRA;
		*internal_format = GL_COMPRESSED_RGBA;
	}
	else
	{
		*format = 0;
		*internal_format = 0;
	}
}

int			bmp_load(const char *imagepath, const char *texture_name)
{
	int				t;
	t_bmp_parser	parser;
	t_texture		*texture;
	GLenum			format[2];

	if (access(imagepath, F_OK | R_OK))
		return (-1);
	if (read_data(&parser, imagepath) == -1)
		return (-1);
	get_format(parser.info.bpp, &format[0], &format[1]);
	t = texture_create(new_vec2(parser.info.width, parser.info.height),
		GL_TEXTURE_2D, format[0], format[1]);
	texture = ezarray_get_index(engine_get()->textures, t);
	texture->name = new_ezstring(texture_name);
	texture->id = hash((unsigned char*)texture_name);
	texture->size.x = parser.info.width;
	texture->size.y = parser.info.height;
	texture->bpp = parser.info.bpp;
	texture->data = parser.data;	
	return (t);
}
