/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/08 16:05:46 by gpinchon         ###   ########.fr       */
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

void	texture_resize(int texture_index, VEC2 new_size)
{
	t_texture	*texture;
	UCHAR		*data;
	char		opp;

	texture = ezarray_get_index(engine_get()->textures, texture_index);
	opp = texture->bpp / 8;
	data = ft_memalloc(new_size.x * new_size.y * opp);

	unsigned x = 0, y;
	while (x < new_size.x)
	{
		float fx = x / (float)new_size.x;
		y = 0;
		while (y < new_size.y)
		{
			float fy = y / (float)new_size.y;
			UCHAR	*new_texel;
			new_texel = &data[y * (int)new_size.x * opp + x * opp];

			UCHAR	*old_texels[4];
			VEC2	old_coords[4];
			float	weights[4];
			old_coords[0] = new_vec2(MIN(texture->width - 1, fx * texture->width), MIN(texture->height - 1, fy * texture->height));
			old_coords[1] = new_vec2(MIN(texture->width - 1, old_coords[0].x + 1), MIN(texture->height - 1, old_coords[0].y + 1));
			old_coords[2] = new_vec2(old_coords[0].x, old_coords[1].y);
			old_coords[3] = new_vec2(old_coords[1].x, old_coords[0].y);
			old_texels[0] = &texture->data[(int)old_coords[0].y * texture->width * opp + (int)old_coords[0].x * opp];
			old_texels[1] = &texture->data[(int)old_coords[1].y * texture->width * opp + (int)old_coords[1].x * opp];
			old_texels[2] = &texture->data[(int)old_coords[2].y * texture->width * opp + (int)old_coords[2].x * opp];
			old_texels[3] = &texture->data[(int)old_coords[3].y * texture->width * opp + (int)old_coords[3].x * opp];
			VEC2	uv = new_vec2(fract(old_coords[0].x), fract(old_coords[0].y));
			weights[0] = ((1 - uv.x) * (1 - uv.y));
			weights[1] = (uv.x * (1 - uv.y));
			weights[2] = ((1 - uv.x) * uv.y);
			weights[3] = (uv.x * uv.y);
			int i = 0;
			while (i < opp)
			{
				int j = 0;
				while(j < 4)
				{
					new_texel[i] += old_texels[j][i] * weights[j];
					j++;
				}
				i++;
			}
			y++;
		}
		x++;
	}
	free(texture->data);
	texture->data = data;
	texture->width = new_size.x;
	texture->height = new_size.y;
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
	if (texture->width > MAXTEXRES || texture->height > MAXTEXRES)
		texture_resize(texture_index, new_vec2(
			texture->width > MAXTEXRES ? MAXTEXRES : texture->width,
			texture->height > MAXTEXRES ? MAXTEXRES : texture->height));
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
