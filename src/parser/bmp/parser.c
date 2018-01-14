/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/13 19:56:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/14 01:11:57 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

# pragma pack(1)
typedef struct	s_bmp_header
{
	char		type[2];
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
	t_bmp_header		header;
	UCHAR				*data;
	unsigned			size_read;
}				t_bmp_parser;

void			convert_bmp(t_bmp_parser *bmp_parser, t_bmp_info *bmp_info)
{
	UCHAR	*pixel_temp = (UCHAR*)calloc(bmp_parser->header.size, sizeof(UCHAR));
	UCHAR	a, b, g, r;
	int		i = 0, j = 0, x, y;

	for (y = 0; y < bmp_info->height; y++)
	{
		for(x = 0; x < bmp_info->width; x++)
		{
			a = bmp_parser->data[i];
			b = bmp_parser->data[i+1];
			g = bmp_parser->data[i+2];
			r = bmp_parser->data[i+3];

			pixel_temp[j] = r;
			pixel_temp[j+1] = g;
			pixel_temp[j+2] = b;
			pixel_temp[j+3] = a;

			i+=4;
			j+=4;
		}
	}
    free(bmp_parser->data);
    bmp_parser->data = pixel_temp;
}

GLuint			load_texture(t_bmp_parser *parser, t_bmp_info *bmp_info)
{
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, bmp_info->bpp == 32 ? GL_RGBA8UI : GL_RGB8UI, bmp_info->width, bmp_info->height, 0, bmp_info->bpp == 32 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, parser->data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return (tex);
}

t_texture	new_texture(const char *name, GLuint oglid)
{
	t_texture	texture;

	texture.id_ogl = oglid;
	texture.name = new_ezstring(name);
	texture.id = hash((unsigned char*)name);
	return (texture);
}

#include <fcntl.h>

int			load_bmp(t_engine *e, const char *imagepath)
{
	t_bmp_parser	parser;
	t_bmp_info		bmp_info;
	GLuint			texture_id;

	if (access(imagepath, F_OK | W_OK))
		return (-1);

	if ((parser.fd = open(imagepath, O_RDONLY)) <= 0)
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
	if (parser.size_read != bmp_info.size)
		return (-1);
	if (bmp_info.bpp == 32)
		convert_bmp(&parser, &bmp_info);
	printf("bpp : %i\nsize : %i\nwidth : %i\nheigth : %i\n", bmp_info.bpp, bmp_info.size, bmp_info.width, bmp_info.height);
	printf("Read Value %i\n", parser.size_read);
	texture_id = load_texture(&parser, &bmp_info);
	free(parser.data);
	t_texture	texture = new_texture(imagepath, texture_id);
	ezarray_push(&e->textures, &texture);
	return (e->textures.length);
}