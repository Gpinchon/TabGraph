/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/15 15:11:03 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/13 16:35:50 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

t_texture	*texture_get(int texture_index)
{
	return (ezarray_get_index(engine_get()->textures, texture_index));
}

void		texture_get_format(int ti, GLenum *format, GLenum *internal_format)
{
	t_texture	*texture;

	texture = texture_get(ti);
	*format = 0;
	*internal_format = 0;
	if (!texture)
		return ;
	*format = texture->format;
	*internal_format = texture->internal_format;
}

UCHAR		texture_get_bpp(int texture_index)
{
	t_texture *texture;

	texture = texture_get(texture_index);
	if (!texture)
		return (0);
	return (texture->bpp);
}

GLuint		texture_get_glid(int texture_index)
{
	t_texture	*texture;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture)
		return (0);
	return (texture->glid);
}

int			texture_get_by_name(char *name)
{
	int			i;
	ULL			h;
	t_texture	*t;

	i = 0;
	h = hash((unsigned char*)name);
	while ((t = texture_get(i)))
	{
		if (h == t->id)
			return (i);
		i++;
	}
	return (-1);
}
