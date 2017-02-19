/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   obj_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/27 20:18:27 by gpinchon          #+#    #+#             */
/*   Updated: 2016/08/11 12:32:21 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scope.h>

typedef struct		s_obj_parser
{
	char			**split;
	char			*mtllib;
	bool			first_object;
	int				fd;
	t_mesh			mesh;
	t_vec3			*v_array;
	t_vec3			*vn_array;
	t_vec3			*vt_array;
	t_material		*mtl;
	GLuint			v_nbr;
	GLuint			vn_nbr;
	GLuint			vt_nbr;
}					t_obj_parser;



int	load_obj(t_engine *engine, char *path)
{
	t_obj_parser	p;

	p.split = split_path(path);
	ezarray_push(engine->meshes, mesh);
	return (p.mesh.length);
}