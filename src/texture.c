/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/17 16:11:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

int		texture_create(VEC2 s, GLenum target, GLenum f, GLenum fi)
{
	t_texture	t;

	ft_memset(&t, 0, sizeof(t_texture));
	t.target = target;
	t.format = f;
	t.internal_format = fi;
	glGenTextures(1, &t.glid);
	glBindTexture(t.target, t.glid);
	if (s.x > 0 && s.y > 0)
		glTexImage2D(GL_TEXTURE_2D, 0, fi, s.x, s.y, 0, f, GL_FLOAT, NULL);
	if (f == GL_RGB)
		glTexParameteri(t.target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexParameteri(t.target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t.target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(t.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glBindTexture(t.target, 0);
	ezarray_push(&engine_get()->textures, &t);
	return (engine_get()->textures.length - 1);
}

void	texture_assign(int ti, int dest_texture_index, GLenum target)
{
	t_texture	*t;
	t_texture	*dest_texture;
	GLenum		format;
	GLenum		internal_format;

	t = texture_get(ti);
	dest_texture = texture_get(dest_texture_index);
	if (!t || !dest_texture)
		return ;
	texture_get_format(ti, &format, &internal_format);
	glBindTexture(dest_texture->target, dest_texture->glid);
	glBindTexture(target, dest_texture->glid);
	glTexImage2D(target, 0, internal_format, t->size.x, t->size.y, 0,
		format, GL_UNSIGNED_BYTE, t->data);
	glBindTexture(target, 0);
	glBindTexture(dest_texture->target, 0);
}

void	texture_load(int ti)
{
	t_texture	*t;
	GLenum		format;
	GLenum		internal_format;

	t = texture_get(ti);
	if (!t || t->loaded)
		return ;
	texture_get_format(ti, &format, &internal_format);
	texture_resize(ti, new_vec2(MIN(t->size.x, MAXTEXRES),
		MIN(t->size.y, MAXTEXRES)));
	glGenTextures(1, &t->glid);
	glBindTexture(t->target, t->glid);
	if (t->bpp < 32)
		glTexParameteri(t->target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexImage2D(t->target, 0, internal_format, t->size.x, t->size.y, 0,
		format, GL_UNSIGNED_BYTE, t->data);
	glGenerateMipmap(t->target);
	glBindTexture(t->target, 0);
	t->loaded = 1;
}

void	texture_generate_mipmap(int texture_index)
{
	t_texture *texture;

	texture = texture_get(texture_index);
	if (!texture)
		return ;
	texture_set_parameters(texture_index, 1,
		(GLenum[1]){GL_TEXTURE_MIN_FILTER},
		(GLenum[1]){GL_LINEAR_MIPMAP_LINEAR});
	glBindTexture(texture->target, texture->glid);
	glGenerateMipmap(texture->target);
	glBindTexture(texture->target, 0);
}
