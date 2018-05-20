/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/13 19:56:09 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/21 01:02:26 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Engine.hpp"
#include "parser/BMP.hpp"
#include "parser/InternalTools.hpp"
#include <stdexcept>
#include <unistd.h>

static void	convert_bmp(t_bmp_parser *parser)
{
	GLubyte	*pixel_temp;
	GLubyte	rgba[4];
	int				i[3];

	auto dataSize = parser->info.bpp / 8 * parser->info.width * parser->info.height;
	pixel_temp = new GLubyte [dataSize];
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
	delete [] parser->data;
	parser->data = pixel_temp;
}

static int	read_data(t_bmp_parser *p, const std::string &path)
{
	unsigned	data_size;

	if (access(path.c_str(), R_OK) != 0) {
		throw std::runtime_error(std::string("Can't access ") + path + " : " + strerror(errno));
	}
	if ((p->fd = fopen(path.c_str(), "rb")) == nullptr) {
		throw std::runtime_error(std::string("Can't open ") + path + " : " + strerror(errno));
	}
	auto readReturn = fread(&p->header, 1, sizeof(p->header), p->fd);
	if (readReturn != sizeof(p->header) || p->header.type != 0x4D42) {
		fclose(p->fd);
		throw std::runtime_error("Wrong Header");
	}
	readReturn = fread(&p->info, 1, sizeof(p->info), p->fd);
	if (readReturn != sizeof(p->info)) {
		fclose(p->fd);
		throw std::runtime_error("Wrong Info");
	}
	data_size = p->info.bpp / 8 * p->info.width * p->info.height;
	fseek(p->fd, p->header.data_offset, SEEK_SET);
	p->data = new GLubyte[data_size];
	p->size_read = fread(p->data, sizeof(unsigned char), data_size, p->fd);
	fclose(p->fd);
	if (p->info.bpp == 32) {
		convert_bmp(p);
	}
	return (0);
}

static void	get_format(GLubyte bpp, GLenum *format, GLenum *internal_format)
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

Texture		*BMP::parse(const std::string &texture_name, const std::string &path)
{
	t_bmp_parser	parser;
	BMP				*texture;
	GLenum			format[2];

	try {
		read_data(&parser, path);
	}
	catch (std::exception &e) {
		throw std::runtime_error(std::string("Error reading ") + path + " :\n" + e.what());
	}
	get_format(parser.info.bpp, &format[0], &format[1]);
	texture = static_cast<BMP*>(Texture::create(texture_name, new_vec2(parser.info.width, parser.info.height),
			GL_TEXTURE_2D, format[0], format[1]));
	texture->_size.x = parser.info.width;
	texture->_size.y = parser.info.height;
	texture->_bpp = parser.info.bpp;
	texture->_data = parser.data;
	return (texture);
}
