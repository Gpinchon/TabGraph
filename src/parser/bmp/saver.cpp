/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   saver.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/23 19:37:43 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 21:24:02 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/BMP.hpp"
#include "parser/InternalTools.hpp"
#include <unistd.h>
#include <sys/stat.h>

static void	prepare_header(t_bmp_header *header, t_bmp_info *info, const Texture &t)
{
	memset(header, 0, sizeof(t_bmp_header));
	memset(info, 0, sizeof(t_bmp_info));
	header->type = 0x4D42;
	header->data_offset = sizeof(t_bmp_header) + sizeof(t_bmp_info);
	header->size = header->data_offset + (t.size().x * t.size().y * 4);
	info->header_size = sizeof(t_bmp_info);
	info->width = t.size().x;
	info->height = t.size().y;
	info->color_planes = 1;
	info->bpp = t.bpp();
	info->size = t.size().x * t.size().y * 4;
	info->horizontal_resolution = 0x0ec4;
	info->vertical_resolution = 0x0ec4;
}

void		BMP::save(const Texture &t, const std::string &imagepath)
{
	t_bmp_header	header;
	t_bmp_info		info;
	GLubyte	*padding;
	int				fd;

	prepare_header(&header, &info, t);
	fd = open(imagepath.c_str(), O_RDWR | O_CREAT | O_BINARY,
		S_IRWXU | S_IRWXG | S_IRWXO);
	write(fd, &header, sizeof(t_bmp_header));
	write(fd, &info, sizeof(t_bmp_info));
	write(fd, t.data(), (t.size().x * t.size().y * t.bpp() / 8));
	padding = new GLubyte[int(info.size - (t.size().x * t.size().y * t.bpp() / 8))]();
	write(fd, padding, info.size - (t.size().x * t.size().y * t.bpp() / 8));
	delete [] padding;
	close(fd);
}
