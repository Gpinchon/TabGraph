/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cubemap.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/16 16:36:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/03/16 16:36:45 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

void	cubemap_load_side(int texture_index, char *path, GLenum side)
{
	texture_assign(load_bmp(path), texture_index, side);
}

int		cubemap_load(char *path, char *n)
{
	int		t;
	char	*b;

	t = texture_create(new_vec2(0, 0), GL_TEXTURE_CUBE_MAP, 0, 0);
	cubemap_load_side(t, b = ft_strjoinfreebool(path, ft_strjoin(n, "/X+.bmp"),
		0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	free(b);
	cubemap_load_side(t, b = ft_strjoinfreebool(path, ft_strjoin(n, "/X-.bmp"),
		0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	free(b);
	cubemap_load_side(t, b = ft_strjoinfreebool(path, ft_strjoin(n, "/Y-.bmp"),
		0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	free(b);
	cubemap_load_side(t, b = ft_strjoinfreebool(path, ft_strjoin(n, "/Y+.bmp"),
		0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	free(b);
	cubemap_load_side(t, b = ft_strjoinfreebool(path, ft_strjoin(n, "/Z+.bmp"),
		0, 1), GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	free(b);
	cubemap_load_side(t, b = ft_strjoinfreebool(path, ft_strjoin(n, "/Z-.bmp"),
		0, 1), GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	free(b);
	texture_generate_mipmap(t);
	return (t);
}
