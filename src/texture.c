/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/12 19:58:44 by gpinchon         ###   ########.fr       */
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
	internal_format = GL_COMPRESSED_RGB;
	if (texture->bpp == 8)
	{
		format = GL_RED;
		internal_format = GL_COMPRESSED_RED;
	}
	else if (texture->bpp == 32)
	{
		format = GL_BGRA;
		internal_format =  GL_COMPRESSED_RGBA;
	}
	glBindTexture(dest_texture->target, dest_texture->id_ogl);
	glBindTexture(target, dest_texture->id_ogl);
	glTexImage2D(target, 0, internal_format, texture->size.x, texture->size.y, 0, format, GL_UNSIGNED_BYTE, texture->data);
	glBindTexture(target, 0);
	glBindTexture(dest_texture->target, 0);
}

VEC4	texture_texelfetch(int texture_index, VEC2 uv)
{
	t_texture	*texture;
	VEC4		value;
	char		opp;
	UCHAR		*texel;
	int			i;

	value = new_vec4(0, 0, 0, 0);
	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture || !texture->data)
		return (value);
	i = 0;
	uv = new_vec2(
		CLAMP(texture->size.x * uv.x, 0, texture->size.x - 1),
		CLAMP(texture->size.y * uv.y, 0, texture->size.y - 1));
	opp = texture->bpp / 8;
	texel = &texture->data[(int)(uv.y * texture->size.x + uv.x) * opp];
	while (i < opp)
	{
		((float*)&value)[i] = texel[i];
		i++;
	}
	return (value);
}

VEC4	texture_sample(int texture_index, VEC2 uv)
{
	t_texture	*t;
	VEC2		coords[4];
	float		weights[4];
	VEC4		value;
	UCHAR		*texel;

	value = new_vec4(0, 0, 0, 0);
	t = ezarray_get_index(engine_get()->textures, texture_index);
	if (!t || !t->data)
		return (value);
	coords[0] = new_vec2(CLAMP(t->size.x * uv.x, 0, t->size.x - 1),
		CLAMP(t->size.y * uv.y, 0, t->size.y - 1));
	coords[1] = new_vec2(MIN(t->size.x - 1, coords[0].x + 1),
		MIN(t->size.y - 1, coords[0].y + 1));
	coords[2] = new_vec2(coords[0].x, coords[1].y);
	coords[3] = new_vec2(coords[1].x, coords[0].y);
	uv = new_vec2(fract(coords[0].x), fract(coords[0].y));
	weights[0] = ((1 - uv.x) * (1 - uv.y));
	weights[1] = (uv.x * (1 - uv.y));
	weights[2] = ((1 - uv.x) * uv.y);
	weights[3] = (uv.x * uv.y);
	int i = 0;
	int j;
	while (i < (t->bpp / 8))
	{
		j = 0;
		while(j < 4)
		{
			texel = &t->data[(int)(coords[j].y * t->size.x + coords[j].x) * (t->bpp / 8)];
			((float*)&value)[i] += texel[i] * weights[j];
			j++;
		}
		i++;
	}
	return (value);
}

void	texture_resize(int texture_index, VEC2 size)
{
	t_texture	*t;
	UCHAR		*d;
	UCHAR		*tex;
	unsigned	x, y;
	VEC2		uv;
	int			i;

	t = ezarray_get_index(engine_get()->textures, texture_index);
	if (t->size.x == size.x && t->size.y == size.y)
		return ;
	d = ft_memalloc(size.x * size.y * (t->bpp / 8));
	x = 0;
	while (x < size.x)
	{
		y = 0;
		while (y < size.y)
		{
			uv = new_vec2(x / (float)size.x, y / (float)size.y);
			VEC4	value = texture_sample(texture_index, uv);
			tex = &d[(int)(y * size.x + x) * (t->bpp / 8)];
			i = 0;
			while (i < (t->bpp / 8))
			{
				tex[i] = ((float*)&value)[i];
				i++;
			}
			y++;
		}
		x++;
	}
	free(t->data);
	t->data = d;
	t->size.x = size.x;
	t->size.y = size.y;
}

void	texture_load(int texture_index)
{
	t_texture	*t;
	GLenum		format;
	GLenum		internal_format;

	t = ezarray_get_index(engine_get()->textures, texture_index);
	if (!t || t->loaded)
		return ;
	format = GL_BGR;
	internal_format = GL_COMPRESSED_RGB;
	if (t->bpp == 8)
	{
		format = GL_RED;
		internal_format = GL_COMPRESSED_RED;
	}
	else if (t->bpp == 32)
	{
		format = GL_BGRA;
		internal_format =  GL_COMPRESSED_RGBA;
	}
	texture_resize(texture_index, new_vec2(MIN(t->size.x, MAXTEXRES),
		MIN(t->size.y, MAXTEXRES)));
	glGenTextures(1, &t->id_ogl);
	glBindTexture(t->target, t->id_ogl);
	glTexParameteri(t->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(t->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	if (t->bpp < 32)
		glTexParameteri(t->target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexImage2D(t->target, 0, internal_format, t->size.x, t->size.y, 0, format, GL_UNSIGNED_BYTE, t->data);
	glGenerateMipmap(t->target);
	glBindTexture(t->target, 0);
	t->loaded = 1;
}

void	texture_generate_mipmap(int texture_index)
{
	t_texture *texture;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	if (!texture)
		return;
	//glGenerateTextureMipmap(texture->id_ogl);
	texture_set_parameters(texture_index, 1,
		(GLenum[1]){GL_TEXTURE_MIN_FILTER},
		(GLenum[1]){GL_LINEAR_MIPMAP_LINEAR});
	glBindTexture(texture->target, texture->id_ogl);
	glGenerateMipmap(texture->target);
	glBindTexture(texture->target, 0);
}
