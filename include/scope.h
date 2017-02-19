/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2017/02/19 01:05:13 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SCOPE_H
# define SCOPE_H

# include <GL/glew.h>
# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
# include <stdint.h>
# include <ezmem.h>
# include <vml.h>
# include <libft.h>

typedef struct	s_point
{
	int			transform_index;
	float		power;
	float		attenuation;
	float		falloff;
	VEC3		color;
}				t_point;

typedef t_point	t_spot;

typedef struct	s_dir
{
	int			transform_index;
	float		power;
	VEC3		color;
}				t_dir;

typedef struct	s_ambient
{
	float		power;
	VEC3		color;
}				t_ambient;

typedef union	u_light_data
{
	t_point		point;
	t_spot		spot;
	t_dir		directional;
	t_ambient	ambient;
}				t_light_data;

typedef struct	s_light
{
	int8_t			type;
	int8_t			cast_shadow;
	t_light_data	data;
}				t_light;

typedef struct	s_material
{
	STRING		name;
	int			id;
	VEC3		base;
	VEC3		ambient;
	VEC2		uv_scale;
	float		roughness;
	float		metalness;
	float		refraction;
	float		alpha;
	float		parallax;
}				t_material;

typedef struct	s_vert_group
{
	int			mtl_id;
	int			mtl_index;
	ARRAY		v;
	ARRAY		vn;
	ARRAY		vt;
	ARRAY		tangent;
	ARRAY		index;
}				t_vert_group;

typedef struct	s_mesh
{
	int			transform_index;
	ARRAY		vert_groups;
}				t_mesh;

typedef struct	s_window
{
	SDL_Window		*sdl_window;
	SDL_GLContext	gl_context;
	VEC4			clear_color;
	GLbitfield		clear_mask;
	float			*display_quad;
}				t_window;

typedef struct	s_engine
{
	int8_t		loop;
	int8_t		swap_interval;
	t_window	*window;
	ARRAY		meshes;
	ARRAY		lights;
	ARRAY		transforms;
}				t_engine;

#endif