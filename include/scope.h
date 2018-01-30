/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/01/30 20:27:48 by gpinchon         ###   ########.fr       */
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
# include <stdio.h>

# define WIDTH	1280
# define HEIGHT	720
# define UP		(VEC3){0, 1, 0}
# define ULL	unsigned long long

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

typedef struct	s_mtl
{
	VEC3		albedo;
	VEC2		uv_scale;
	float		roughness;
	float		metallic;
	float		refraction;
	float		alpha;
	float		parallax;
	int			texture_albedo;
	int			texture_roughness;
	int			texture_metallic;
	int			texture_normal;
	int			texture_height;
}				t_mtl;

typedef struct	s_texture
{
	ULL			id;
	GLuint		id_ogl;
	STRING		name;
	int			width;
	int			height;
	char		bpp;
	GLenum		target;
	UCHAR		*data;
	BOOL		loaded;
}				t_texture;

typedef struct	s_shadervariable
{
	ULL			id;
	STRING		name;
	GLint		size;
	GLenum		type;
	GLuint		loc;
}				t_shadervariable;

typedef struct	s_shader
{
	ULL			id;
	STRING		name;
	GLuint		program;
	ARRAY		uniforms;
	ARRAY		attributes;
}				t_shader;

typedef struct	s_material
{
	ULL			id;
	STRING		name;
	t_mtl		data;
}				t_material;

typedef struct	s_vgroup
{
	ULL			mtl_id;
	int			mtl_index;
	int			shader_index;
	int			in_campos;
	int			in_transform;
	int			in_modelmatrix;
	int			in_normalmatrix;
	int			in_albedo;
	int			in_uvmin;
	int			in_uvmax;
	int			in_roughness;
	int			in_metallic;
	int			in_refraction;
	int			in_alpha;
	int			in_parallax;
	int			in_use_texture_albedo_alpha;
	int			in_texture_albedo;
	int			in_use_texture_albedo;
	int			in_texture_roughness;
	int			in_use_texture_roughness;
	int			in_texture_metallic;
	int			in_use_texture_metallic;
	int			in_texture_normal;
	int			in_use_texture_normal;
	int			in_texture_height;
	int			in_use_texture_height;
	int			in_texture_env;
	int			in_texture_env_spec;
	ARRAY		v;
	ARRAY		vn;
	ARRAY		vt;
	VEC2		uvmax;
	VEC2		uvmin;
	GLuint		v_arrayid;
	GLuint		v_bufferid;
	GLuint		vn_bufferid;
	GLuint		vt_bufferid;
}				t_vgroup;

typedef struct	s_aabb
{
	VEC3		min;
	VEC3		max;
	VEC3		center;
}				t_aabb;

typedef struct	s_mesh
{
	int			transform_index;
	t_aabb		bounding_box;
	ARRAY		vgroups;
}				t_mesh;

typedef struct	s_camera
{
	VEC3		position;
	MAT4		view;
	MAT4		projection;
	FRUSTUM		frustum;
	float		fov;
	int			target_index;
}				t_camera;

typedef struct	s_window
{
	SDL_Window		*sdl_window;
	SDL_GLContext	gl_context;
	VEC4			clear_color;
	GLbitfield		clear_mask;
	float			*display_quad;
}				t_window;

struct s_engine;
typedef void(*kcallback)(struct s_engine *engine, SDL_Event *event);

typedef struct	s_engine
{
	int8_t		loop;
	int8_t		swap_interval;
	t_window	*window;
	ARRAY		cameras;
	ARRAY		meshes;
	ARRAY		lights;
	ARRAY		transforms;
	ARRAY		materials;
	ARRAY		shaders;
	ARRAY		textures;
	int			env;
	int			env_spec;
	int			delta_time;
	kcallback	kcallbacks[285];
}				t_engine;

char	*g_program_path;


t_material	new_material();
int			get_material_index_by_name(ARRAY materials, char *name);
int			get_material_index_by_id(ARRAY materials, ULL	h);

t_texture	new_texture(const char *name);

/*
** Parser tools
*/

char			**split_path(const char *path);
ULL	hash(unsigned char *str);

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

/*
** .bmp parser
*/

int			load_bmp(t_engine *e, const char *imagepath);


int		load_shaders(t_engine *engine, const char *name, const char *vertex_file_path,const char *fragment_file_path);
int		transform_create(t_engine *e, VEC3 position, VEC3 rotation, VEC3 scale);

#endif