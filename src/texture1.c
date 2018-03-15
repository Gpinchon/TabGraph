/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/15 15:11:03 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/15 15:11:22 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

t_texture	*texture_get(int texture_index)
{
	return (ezarray_get_index(engine_get()->textures, texture_index));
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