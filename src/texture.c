/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 17:03:48 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/15 18:01:34 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

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

void	texture_set_parameters(int ti, int p_nbr, GLenum *p, GLenum *v)
{
	t_texture *texture;

	texture = texture_get(ti);
	if (!texture)
		return ;
	glBindTexture(texture->target, texture->glid);
	while (p_nbr > 0)
	{
		glTexParameteri(texture->target, p[p_nbr - 1], v[p_nbr - 1]);
		p_nbr--;
	}
	glBindTexture(texture->target, 0);
}

int		texture_create(VEC2 s, GLenum target, GLenum fi, GLenum f)
{
	t_texture	t;

	ft_memset(&t, 0, sizeof(t_texture));
	t.target = target;
	glGenTextures(1, &t.glid);
	glBindTexture(t.target, t.glid);
	if (s.x > 0 && s.y > 0)
		glTexImage2D(GL_TEXTURE_2D, 0, fi, s.x, s.y, 0, f, GL_FLOAT, NULL);
	if (f == GL_RGB)
		glTexParameteri(t.target, GL_TEXTURE_SWIZZLE_A, GL_ONE);
	glTexParameterf(t.target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
	glBindTexture(t.target, 0);
	ezarray_push(&engine_get()->textures, &t);
	return (engine_get()->textures.length - 1);
}

void	texture_get_format(int ti, GLenum *format, GLenum *internal_format)
{
	t_texture	*texture;

	texture = texture_get(ti);
	*format = 0;
	*internal_format = 0;
	if (!texture)
		return ;
	if (texture->bpp == 8)
	{
		*format = GL_RED;
		*internal_format = GL_COMPRESSED_RED;
	}
	else if (texture->bpp == 24)
	{
		*format = GL_BGR;
		*internal_format = GL_COMPRESSED_RGB;
	}
	else if (texture->bpp == 32)
	{
		*format = GL_BGRA;
		*internal_format = GL_COMPRESSED_RGBA;
	}
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

VEC4	texture_texelfetch(int texture_index, VEC2 uv)
{
	t_texture		*texture;
	VEC4			value;
	char			opp;
	unsigned char	*p;
	int				i;

	value = new_vec4(0, 0, 0, 0);
	texture = texture_get(texture_index);
	if (!texture || !texture->data)
		return (value);
	i = 0;
	uv = new_vec2(
		CLAMP(floor(texture->size.x * uv.x), 0, texture->size.x - 1),
		CLAMP(floor(texture->size.y * uv.y), 0, texture->size.y - 1));
	opp = texture->bpp / 8;
	p = &texture->data[(int)(uv.y * texture->size.x + uv.x) * opp];
	while (i < opp)
	{
		((float*)&value)[i] = p[i];
		i++;
	}
	return (value);
}

VEC4	texture_sample(int texture_index, VEC2 uv)
{
	t_texture	*t;
	int			s[2];
	VEC3		vt[4];
	VEC4		value;

	value = new_vec4(0, 0, 0, 0);
	if (!(t = texture_get(texture_index)) || !t->data)
		return (value);
	vt[0] = new_vec3(CLAMP(t->size.x * uv.x, 0, t->size.x - 1),
		CLAMP(t->size.y * uv.y, 0, t->size.y - 1), 0);
	uv = new_vec2(fract(vt[0].x), fract(vt[0].y));
	vt[0].z = ((1 - uv.x) * (1 - uv.y));
	vt[1] = new_vec3(MIN(t->size.x - 1, vt[0].x + 1),
		MIN(t->size.y - 1, vt[0].y + 1), (uv.x * (1 - uv.y)));
	vt[2] = new_vec3(vt[0].x, vt[1].y, ((1 - uv.x) * uv.y));
	vt[3] = new_vec3(vt[1].x, vt[0].y, (uv.x * uv.y));
	s[0] = -1;
	while (++s[0] < (t->bpp / 8))
	{
		s[1] = -1;
		while (++s[1] < 4)
			((float*)&value)[s[0]] += (&t->data[(int)(floor(vt[s[1]].y) *
			t->size.x + floor(vt[s[1]].x)) * (t->bpp / 8)])[s[0]] * vt[s[1]].z;
	}
	return (value);
}

void	texture_resize(int texture_index, VEC2 ns)
{
	t_texture		*t;
	unsigned char	*d;
	int				i[3];
	VEC4			v;
	VEC2			uv;

	t = texture_get(texture_index);
	d = ft_memalloc(ns.x * ns.y * (t->bpp / 8));
	i[0] = -1;
	while (++i[0] < ns.x)
	{
		i[1] = -1;
		while (++i[1] < ns.y)
		{
			uv = new_vec2(i[0] / (float)ns.x, i[1] / (float)ns.y);
			v = texture_sample(texture_index, uv);
			i[2] = -1;
			while (++i[2] < (t->bpp / 8))
				(&d[(int)(i[1] * ns.x + i[0]) * (t->bpp / 8)])[i[2]] =
				((float*)&v)[i[2]];
		}
	}
	free(t->data);
	t->data = d;
	t->size = ns;
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
	glTexParameteri(t->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(t->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(t->target, GL_TEXTURE_MAX_ANISOTROPY_EXT, ANISOTROPY);
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
