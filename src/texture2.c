/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   texture2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/15 18:20:54 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/10 16:18:13 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

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
		CLAMP(round(texture->size.x * uv.x), 0, texture->size.x - 1),
		CLAMP(round(texture->size.y * uv.y), 0, texture->size.y - 1));
	opp = texture->bpp / 8;
	p = &texture->data[(int)(uv.y * texture->size.x + uv.x) * opp];
	while (i < opp)
	{
		((float*)&value)[i] = p[i];
		i++;
	}
	return (value);
}

void	texture_set_pixel(int texture_index, VEC2 uv, VEC4 value)
{
	t_texture		*texture;
	char			opp;
	unsigned char	*p;
	int				i;

	texture = texture_get(texture_index);
	if (!texture || !texture->data)
		return ;
	i = 0;
	uv = new_vec2(
		CLAMP(round(texture->size.x * uv.x), 0, texture->size.x - 1),
		CLAMP(round(texture->size.y * uv.y), 0, texture->size.y - 1));
	opp = texture->bpp / 8;
	p = &texture->data[(int)(uv.y * texture->size.x + uv.x) * opp];
	while (i < opp)
	{
		p[i] = ((float*)&value)[i] * 255.f;
		i++;
	}
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
			((float*)&value)[s[0]] += (&t->data[(int)(round(vt[s[1]].y) *
			t->size.x + round(vt[s[1]].x)) * (t->bpp / 8)])[s[0]] * vt[s[1]].z;
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
	d = calloc(ns.x * ns.y * (t->bpp / 8), sizeof(UCHAR));
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
