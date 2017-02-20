/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2017/02/20 20:52:47 by gpinchon         ###   ########.fr       */
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

typedef struct	s_pbrmtl
{
	VEC3		base;
	VEC3		ambient;
	VEC3		specular;
	//VEC2		uv_scale;
	float		roughness;
	float		metalness;
	float		refraction;
	float		alpha;
	float		parallax;
}				t_pbrmtl;

typedef struct	s_blinmtl
{
	VEC3		diffuse;
	VEC3		ambient;
	VEC3		specular;
	//VEC2		uv_scale;
	float		spec_pow;
	float		refraction;
	float		alpha;
	float		parallax;
}				t_blinmtl;

typedef union	s_mtl_data
{
	t_blinmtl	blin;
	t_pbrmtl	pbr;
}				t_mtl_data;

typedef struct	s_material
{
	STRING		name;
	int8_t		type;
	unsigned long	id;
	t_mtl_data	data;
}				t_material;

typedef struct	s_vgroup
{
	unsigned long	mtl_id;
	int			mtl_index;
	ARRAY		v;
	ARRAY		vn;
	ARRAY		vt;
	ARRAY		tangent;
	ARRAY		vindex;
}				t_vgroup;

typedef struct	s_mesh
{
	int			transform_index;
	ARRAY		vgroups;
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
	ARRAY		materials;
}				t_engine;

t_material	new_material();
int			get_material_index_by_name(ARRAY materials, char *name);
int			get_material_index_by_id(ARRAY materials, unsigned long	h);

/*
** Parser tools
*/

char			**split_path(const char *path);
unsigned long	hash(unsigned char *str);

/*
** .mtllib parser
*/
int		load_mtllib(t_engine *engine, char *path);

/*
** .obj parser
*/

int		load_obj(t_engine *engine, char *path);
VEC3	parse_vec3(char **split);
VEC2	parse_vec2(char **split);



#endif