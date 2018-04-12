/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/12 19:49:12 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/12 19:49:35 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../parser.h"

int	load_mtllib(char *path)
{
	t_obj_parser	p;

	memset(&p, 0, sizeof(t_obj_parser));
	path = ft_strjoin(engine_get()->exec_path, path);
	if (start_mtllib_parsing(&p, path))
		return (-1);
	free(path);
	return (engine_get()->materials.length);
}