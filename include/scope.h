/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scope.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/02 16:55:54 by gpinchon         ###   ########.fr       */
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
# include <fcntl.h>

# define WIDTH		1280
# define HEIGHT		720
# define IWIDTH		1024
# define IHEIGHT	1024
# define SHADOWRES	2048
# define UP			(VEC3){0, 1, 0}
# define ULL		unsigned long long
# define ANISOTROPY	8.f
# define MSAA		4
# define BLOOMPASS	2
# define MAXTEXRES	1024
# ifndef O_BINARY
#  define O_BINARY 0x0004
# endif
# define MIN(x, y) (x < y ? x : y)
# define MAX(x, y) (x > y ? x : y)
# define RENDERTYPE	enum e_rendertype
# define LDATA		t_light_data
# define GLCONTEXT	SDL_GLContext

/*
** Key binding
*/

# define DOWNK		SDL_SCANCODE_DOWN
# define UPK		SDL_SCANCODE_UP
# define LEFTK		SDL_SCANCODE_LEFT
# define RIGHTK		SDL_SCANCODE_RIGHT
# define ZOOMK		SDL_SCANCODE_KP_PLUS
# define UNZOOMK	SDL_SCANCODE_KP_MINUS

enum			e_rendertype
{
	render_all = 0, render_opaque = 1, render_transparent = 2
};

typedef void	(*t_kcallback)(SDL_Event *event);

typedef struct	s_framebuffer
{
	GLuint		id;
	ARRAY		color_attachements;
	short		depth;
	short		shader;
	VEC2		size;
}				t_framebuffer;

typedef struct	s_point
{
	VEC3		color;
	float		power;
	float		attenuation;
	float		falloff;
}				t_point;

typedef t_point	t_spot;

typedef struct	s_dir
{
	VEC3		color;
	float		power;
}				t_dir;

typedef union	u_light_data
{
	t_point		point;
	t_spot		spot;
	t_dir		directional;
}				t_light_data;

typedef struct	s_light
{
	short		transform_index;
	int8_t		type;
	int8_t		cast_shadow;
	LDATA		data;
	short		render_buffer;
}				t_light;

typedef struct	s_mtl
{
	VEC3		albedo;
	VEC3		specular;
	VEC3		emitting;
	VEC2		uv_scale;
	float		roughness;
	float		metallic;
	float		alpha;
	float		parallax;
	float		stupidity;
	short		texture_albedo;
	short		texture_specular;
	short		texture_roughness;
	short		texture_metallic;
	short		texture_emitting;
	short		texture_normal;
	short		texture_height;
	short		texture_ao;
	short		texture_stupid;
}				t_mtl;

typedef struct	s_material
{
	ULL			id;
	STRING		name;
	t_mtl		data;
	short		shader_index;
	short		shader_in[36];
}				t_material;

typedef struct	s_texture
{
	ULL			id;
	GLuint		glid;
	STRING		name;
	VEC2		size;
	char		bpp;
	GLenum		target;
	GLenum		format;
	GLenum		internal_format;
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

typedef struct	s_aabb
{
	VEC3		min;
	VEC3		max;
	VEC3		center;
}				t_aabb;

typedef struct	s_vgroup
{
	ULL			mtl_id;
	short		mtl_index;
	t_aabb		bounding_box;
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

typedef struct	s_mesh
{
	short		transform_index;
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
	short		target_index;
}				t_camera;

typedef struct	s_window
{
	SDL_Window	*sdl_window;
	GLCONTEXT	gl_context;
	VEC4		clear_color;
	GLbitfield	clear_mask;
	short		render_buffer;
}				t_window;

typedef struct	s_engine
{
	int8_t		loop;
	int8_t		swap_interval;
	ARRAY		cameras;
	ARRAY		meshes;
	ARRAY		lights;
	ARRAY		transforms;
	ARRAY		materials;
	ARRAY		shaders;
	ARRAY		textures;
	ARRAY		textures_env;
	ARRAY		framebuffers;
	short		env;
	short		env_spec;
	short		brdf_lut;
	float		delta_time;
	float		stupidity;
	float		new_stupidity;
	t_kcallback	kcallbacks[285];
	t_kcallback	rcallback;
	char		*program_path;
}				t_engine;

/*
** Engine functions
*/

t_engine		*engine_get();
void			engine_destroy();
void			engine_init();
void			engine_load_env();
void			set_key_callback(SDL_Scancode keycode, t_kcallback callback);
void			set_refresh_callback(t_kcallback callback);

/*
** Window functions
*/

t_window		*window_get();
VEC2			window_get_size();
void			window_init(const char *name, int width, int height);
void			window_fullscreen(char fullscreen);

/*
** Event functions
*/
void			event_window(SDL_Event *event);
void			event_keyboard(SDL_Event *event);
int				event_callback(void *e, SDL_Event *event);
int				event_refresh(void);

/*
** Cubemap functions
*/
void			cubemap_load_side(int texture_index, char *path, GLenum side);
int				cubemap_load(char *path, char *n);

/*
** Camera functions
*/
int				camera_get_target_index(int camera_index);
int				camera_create(float fov);
void			camera_set_target(int camera_index, int transform_index);
void			camera_set_position(int camera_index, VEC3 position);
void			camera_orbite(int camera_index, float phi,
				float theta, float radius);
void			camera_update(int camera_index);

/*
** Material functions
*/
int				material_create(char *name);
int				material_get_index_by_name(char *name);
int				material_get_index_by_id(ULL h);
void			material_assign_shader(int material_index, int shader_index);
void			material_bind_textures(int material_index);
void			material_set_uniforms(int material_index);
void			material_load_textures(int material_index);
void			material_set_albedo(int material_index, VEC3 value);
void			material_set_specular(int material_index, VEC3 value);
void			material_set_emitting(int material_index, VEC3 value);
void			material_set_uv_scale(int material_index, VEC2 value);
void			material_set_roughness(int material_index, float value);
void			material_set_metallic(int material_index, float value);
void			material_set_alpha(int material_index, float value);
void			material_set_parallax(int material_index, float value);
void			material_set_stupidity(int material_index, float value);
void			material_set_texture_albedo(int material_index, int value);
void			material_set_texture_specular(int material_index, int value);
void			material_set_texture_roughness(int material_index, int value);
void			material_set_texture_metallic(int material_index, int value);
void			material_set_texture_emitting(int material_index, int value);
void			material_set_texture_normal(int material_index, int value);
void			material_set_texture_height(int material_index, int value);
void			material_set_texture_ao(int material_index, int value);
void			material_set_texture_stupid(int material_index, int value);
VEC3			material_get_albedo(int material_index);
VEC3			material_get_specular(int material_index);
VEC3			material_get_emitting(int material_index);
VEC2			material_get_uv_scale(int material_index);
float			material_get_roughness(int material_index);
float			material_get_metallic(int material_index);
float			material_get_alpha(int material_index);
float			material_get_parallax(int material_index);
float			material_get_stupidity(int material_index);
int				material_get_texture_albedo(int material_index);
int				material_get_texture_roughness(int material_index);
int				material_get_texture_metallic(int material_index);
int				material_get_texture_emitting(int material_index);
int				material_get_texture_normal(int material_index);
int				material_get_texture_height(int material_index);
int				material_get_texture_ao(int material_index);
int				material_get_texture_stupid(int material_index);

/*
** Mesh functions
*/
void			mesh_update(int mesh_index);
void			mesh_rotate(int mesh_index, VEC3 rotation);
void			mesh_scale(int mesh_index, VEC3 scale);
void			mesh_translate(int mesh_index, VEC3 position);
void			mesh_load(int mesh_index);
void			mesh_center(int mesh_index);
void			mesh_render(int mesh_index, int camera_index, RENDERTYPE type);

/*
** Vgroup functions
*/

void			vgroup_load(int mesh_index, int vgroup_index);
void			vgroup_render(int camera_index, int mesh_index,
					int vgroup_index);
void			vgroup_center(int mesh_index, int vgroup_index);

/*
** Texture functions
*/

t_texture		*texture_get(int texture_index);
void			texture_resize(int texture_index, VEC2 new_size);
void			texture_get_format(int texture_index, GLenum *format,
					GLenum *internal_format);
GLuint			texture_get_glid(int texture_index);
UCHAR			texture_get_bpp(int texture_index);
int				texture_get_by_name(char *name);
int				texture_create(VEC2 size, GLenum target, GLenum format,
					GLenum internal_format);
void			texture_set_parameters(int texture_index, int parameter_nbr,
					GLenum *parameters, GLenum *values);
void			texture_assign(int texture_index, int dest_texture_index,
					GLenum target);
void			texture_load(int texture_index);
void			texture_generate_mipmap(int texture_index);
void			texture_set_pixel(int texture_index, VEC2 uv, VEC4 value);
VEC4			texture_texelfetch(int texture_index, VEC2 uv);
int				texture_generate_brdf();

/*
** Parser tools
*/

char			**split_path(const char *path);
ULL				hash(unsigned char *str);
char			*convert_backslash(char *str);

/*
** .mtllib parser
*/
int				load_mtllib(char *path);

/*
** .obj parser
*/

int				load_obj(char *path);
VEC3			parse_vec3(char **split);
VEC2			parse_vec2(char **split);

/*
** .bmp parser
*/

int				bmp_load(const char *imagepath);
void			bmp_save(int texture, const char *imagepath);

/*
** Shader functions
*/

void			shader_bind_texture(int shader_index, int uniform_index,
					int texture_index, GLenum texture_unit);
void			shader_unbind_texture(int shader_index, GLenum texture_unit);
void			shader_set_int(int shader_index, int uniform_index, int value);
void			shader_set_uint(int shader_index, int uniform_index,
					unsigned value);
void			shader_set_float(int shader_index, int uniform_index,
					float value);
void			shader_set_vec2(int shader_index, int uniform_index, VEC2 val);
void			shader_set_vec3(int shader_index, int uniform_index, VEC3 val);
void			shader_set_mat4(int shader_index, int uniform_index, MAT4 val);
void			shader_use(int shader_index);
int				shader_get_uniform_index(int shader_index, char *name);
int				shader_get_by_name(char *name);
int				load_shaders(const char *name, const char *vertex_file_path,
					const char *fragment_file_path);

/*
** Transform functions
*/

int				transform_create(VEC3 position, VEC3 rotation, VEC3 scale);
VEC3			transform_set_position(int transform_index, VEC3 position);
VEC3			transform_get_position(int transform_index);

/*
** Framebuffer functions
*/

void			framebuffer_bind(int framebuffer);
int				framebuffer_get_attachement(int framebuffer,
					int color_attachement);
int				framebuffer_get_depth(int framebuffer);
int				framebuffer_get_shader(int framebuffer);
int				framebuffer_create(VEC2 size, int shader,
					int color_attachements, int depth);
void			framebuffer_setup_attachements(int fi);
int				framebuffer_create_attachement(int framebuffer,
					GLenum format, GLenum iformat);

/*
** Callback functions
*/

void			callback_refresh(SDL_Event *event);
void			callback_scale(SDL_Event *event);
void			callback_stupidity(SDL_Event *event);
void			callback_background(SDL_Event *event);
void			callback_exit(SDL_Event *event);
void			callback_fullscreen(SDL_Event *event);
void			callback_camera(SDL_Event *event);

#endif
