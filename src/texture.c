/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/22 22:08:16 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

GLuint	texture_get_ogl_id(int texture_index)
{
	t_texture	*texture;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture)
		return (0);
	return (texture->id_ogl);
}

int		texture_get_by_name(char *name)
{
	int			i;
	ULL			h;
	t_texture	*t;

	i = 0;
	h = hash((unsigned char*)name);
	while ((t = ezarray_get_index(engine_get()->textures, i)))
	{
		if (h == t->id)
			return (i);
		i++;
	}
	return (-1);
}

void	texture_set_parameters(int texture_index, int parameter_nbr, GLenum *parameters, GLenum *values)
{
	t_texture *texture;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture)
		return;
	glBindTexture(texture->target, texture->id_ogl);
	while (parameter_nbr > 0)
	{
		glTexParameteri(texture->target, parameters[parameter_nbr - 1], values[parameter_nbr - 1]);
		parameter_nbr--;
	}
	glBindTexture(texture->target, 0);
}

int		texture_create(VEC2 size, GLenum target, GLenum internal_format, GLenum format)
{
	t_texture	texture;

	ft_memset(&texture, 0, sizeof(t_texture));
	texture.target = target;
	glGenTextures(1, &texture.id_ogl);
	glBindTexture(texture.target, texture.id_ogl);
	if (size.x > 0 && size.y > 0)
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, size.x, size.y, 0, format, GL_FLOAT, NULL);
	if (format == GL_RGB)
		glTexParameteri(texture.target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexParameterf(texture.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glBindTexture(texture.target, 0);
	ezarray_push(&engine_get()->textures, &texture);
	return (engine_get()->textures.length - 1);
}

void	texture_assign(int texture_index, int dest_texture_index, GLenum target)
{
	t_texture	*texture;
	t_texture	*dest_texture;
	GLenum format;
	GLenum internal_format;
	
	texture = ezarray_get_index(engine_get()->textures, texture_index);
	dest_texture = ezarray_get_index(engine_get()->textures, dest_texture_index);
	if (!texture || !dest_texture)
		return ;
	format = GL_BGR;
	internal_format = GL_RGB;
	if (texture->bpp == 8)
	{
		format = GL_RED;
		internal_format = GL_RED;
	}
	else if (texture->bpp == 32)
	{
		format = GL_BGRA;
		internal_format =  GL_RGBA;
	}
	glBindTexture(dest_texture->target, dest_texture->id_ogl);
	glBindTexture(target, dest_texture->id_ogl);
	glTexImage2D(target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glBindTexture(target, 0);
	glBindTexture(dest_texture->target, 0);
	return ;
}

void	texture_load(int texture_index)
{
	t_texture	*texture;
	GLenum		format;
	GLenum		internal_format;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture || texture->loaded)
		return ;
	format = GL_BGR;
	internal_format = GL_RGB;
	if (texture->bpp == 8)
	{
		format = GL_RED;
		internal_format = GL_RED;
	}
	else if (texture->bpp == 32)
	{
		format = GL_BGRA;
		internal_format =  GL_RGBA;
	}
	glGenTextures(1, &texture->id_ogl);
	glBindTexture(texture->target, texture->id_ogl);
	glTexParameteri(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(texture->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.f);
	if (texture->bpp < 32)
		glTexParameteri(texture->target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexImage2D(texture->target, 0, internal_format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glGenerateMipmap(texture->target);
	glBindTexture(texture->target, 0);
	texture->loaded = 1;
	return ;
}

void	texture_generate_mipmap(int texture_index)
{
	t_texture *texture;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture)
		return;
	texture_set_parameters(texture_index, 1,
		(GLenum[1]){GL_TEXTURE_MIN_FILTER},
		(GLenum[1]){GL_LINEAR_MIPMAP_LINEAR});
	glBindTexture(texture->target, texture->id_ogl);
	glGenerateMipmap(texture->target);
	glBindTexture(texture->target, 0);
}
