/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/02/09 15:58:46 by gpinchon         ###   ########.fr       */
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
# define ANISOTROPY	16.f
# define MSAA		8

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
	VEC3		emitting;
	VEC2		uv_scale;
	float		roughness;
	float		metallic;
	float		refraction;
	float		alpha;
	float		parallax;
	int			texture_albedo;
	int			texture_roughness;
	int			texture_metallic;
	int			texture_emitting;
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
	int			shader_index;
	int			in_campos;
	int			in_transform;
	int			in_modelmatrix;
	int			in_normalmatrix;
	int			in_albedo;
	int			in_emitting;
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
	int			in_texture_emitting;
	int			in_use_texture_emitting;
	int			in_texture_normal;
	int			in_use_texture_normal;
	int			in_texture_height;
	int			in_use_texture_height;
	int			in_texture_env;
	int			in_texture_env_spec;
}				t_material;

typedef struct	s_vgroup
{
	ULL			mtl_id;
	int			mtl_index;
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

typedef struct	s_framebuffer
{
	GLuint		id;
	int			texture_color;
	int			texture_normal;
	int			texture_position;
	int			texture_depth;
}				t_framebuffer;

typedef struct	s_window
{
	SDL_Window		*sdl_window;
	SDL_GLContext	gl_context;
	VEC4			clear_color;
	GLbitfield		clear_mask;
	float			*display_quad;
	int				render_shader;
	t_framebuffer	render_buffer;	
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

/*
** Engine functions
*/

t_engine	*engine_init();
void		engine_load_env(t_engine *e);
void		engine_set_key_callback(t_engine *engine, SDL_Scancode keycode, kcallback callback);

/*
** Camera functions
*/
int			camera_get_target_index(t_engine *engine, int camera_index);
int			camera_create(t_engine *engine, float fov);
void		camera_set_target(t_engine *engine, int camera_index, int transform_index);
void		camera_set_position(t_engine *engine, int camera_index, VEC3 position);
void		camera_orbite(t_engine *engine, int camera_index, float phi, float theta, float radius);
void		camera_update(t_engine *engine, int camera_index);

/*
** Material functions
*/
t_material	new_material(char *name);
int		material_get_index_by_name(t_engine *engine, char *name);
int		material_get_index_by_id(t_engine *engine, ULL h);
void	material_assign_shader(t_engine *engine, int material_index, int shader_index);
void	material_set_textures(t_engine *engine, int material_index);
void	material_set_uniforms(t_engine *engine, int material_index);
void	material_load_textures(t_engine *engine, int material_index);

/*
** Mesh functions
*/
void	mesh_rotate(t_engine *engine, int mesh_index, VEC3 rotation);
void	mesh_scale(t_engine *engine, int mesh_index, VEC3 scale);
void	mesh_translate(t_engine *engine, int mesh_index, VEC3 position);
void	mesh_load(t_engine *engine, int mesh_index);
void	mesh_center(t_engine *engine, int mesh_index);
void	mesh_render(t_engine *engine, int camera_index, int mesh_index);

/*
** Vgroup functions
*/

void	vgroup_load(t_engine *engine, int mesh_index, int vgroup_index);
void	vgroup_render(t_engine *engine, int camera_index, int mesh_index, int vgroup_index);
void	vgroup_center(t_engine *engine, int mesh_index, int vgroup_index);

/*
** Texture functions
*/

GLuint		texture_get_ogl_id(t_engine *engine, int texture_index);
int			texture_create(t_engine *engine, VEC2 size, GLenum target, GLenum internal_format, GLenum format);
void		texture_set_parameters(t_engine *engine, int texture_index, int parameter_nbr, GLenum *parameters, GLenum *values);
void		texture_assign(t_engine *engine, int texture_index, int dest_texture_index, GLenum target);
void		texture_load(t_engine *engine, int texture_index);
void		texture_generate_mipmap(t_engine *engine, int texture_index);

/*
** Parser tools
*/

char		**split_path(const char *path);
ULL			hash(unsigned char *str);
char		*convert_backslash(char *str);

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

/*
** Shader functions
*/

void	shader_set_texture(t_engine *engine, int shader_index, int uniform_index, int texture_index, GLenum texture_unit);
void	shader_set_uniform(t_engine *engine, int shader_index, int uniform_index, void *value);
void	shader_use(t_engine *engine, int shader_index);
int		shader_get_uniform_index(t_engine *engine, int shader_index, char *name);
int		shader_get_by_name(t_engine *engine, char *name);
int		load_shaders(t_engine *engine, const char *name, const char *vertex_file_path,const char *fragment_file_path);
int		transform_create(t_engine *e, VEC3 position, VEC3 rotation, VEC3 scale);

#endif