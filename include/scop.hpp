/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scop.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/05 14:17:09 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef SCOP_H
# define SCOP_H

# include <GL/glew.h>
# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
# include <stdint.h>
# include <vml.h>
# include <iostream>
# include <fcntl.h>
# include <stdio.h>
# include <vector>
# include <map>
# include <string>
# include <functional>

# define WIDTH		1280
# define HEIGHT		720
# define SHADOWRES	2048
# define UP			(VEC3){0, 1, 0}
# define ULL		unsigned long long
# define ANISOTROPY	4.f
# define MSAA		4
# define BLOOMPASS	2
# define MAXTEXRES	1024
# ifndef O_BINARY
#  define O_BINARY 0x0004
# endif
# define MIN(x, y) (x <= y ? x : y)
# define MAX(x, y) (x >= y ? x : y)
# define RENDERTYPE	enum e_rendertype
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
# define UCHAR		unsigned char

class Texture;

enum			e_rendertype
{
	render_all = 0, render_opaque = 1, render_transparent = 2
};

struct	BoundingElement
{
	bool collides(const BoundingElement &) { return (false); };
	VEC3	min;
	VEC3	max;
	VEC3	center;
};

struct	AABB : public BoundingElement
{
	bool	collides(const AABB &) { return (false); };
};

typedef void	(*t_kcallback)(SDL_Event *event);

struct	ShaderVariable
{
	ULL			id;
	std::string	name;
	GLint		size;
	GLenum		type;
	GLuint		loc;
};

class Shader
{
public :
	static Shader	*get_by_name(const std::string &);
	static Shader	*load(const std::string &name, const std::string &vertex_file_path,
						const std::string &fragment_file_path);
	static bool		check_shader(const GLuint &id);
	static bool		check_program(const GLuint &id);
	GLuint			link(const GLuint &vertexid, const GLuint &fragmentid);
	void			bind_texture(const std::string &,
					const Texture *, const GLenum &texture_unit);
	void			unbind_texture(GLenum texture_unit);
	void			set_name(const std::string &);
	void			set_uniform(const std::string &name, const Texture *, const int value);
	void			set_uniform(const std::string &, const int value);
	void			set_uniform(const std::string &, const unsigned &value);
	void			set_uniform(const std::string &, const float &value);
	void			set_uniform(const std::string &, const VEC2 &val);
	void			set_uniform(const std::string &, const VEC3 &val);
	void			set_uniform(const std::string &, const MAT4 &val);
	void			use(const bool &use_program = true);
	ShaderVariable	*get_uniform(const std::string &name);
	const std::string &name();
	bool			in_use();
private :
	std::map<std::string, ShaderVariable>	_get_variables(GLenum type);
	ULL			_id;
	std::string	_name;
	GLuint		_program;
	bool		_in_use;
	std::map<std::string, ShaderVariable> _uniforms;
	std::map<std::string, ShaderVariable> _attributes;
	Shader(const std::string &name);
};

class	Texture
{
public :
	static Texture	*create(const std::string &name, VEC2 size, GLenum target, GLenum format, GLenum internal_format);
	static Texture	*get_by_name(const std::string &);
	void			resize(const VEC2 &new_size);
	void			set_name(const std::string &);
	void			set_parameter(GLenum p, GLenum v);
	void			set_parameters(int parameter_nbr,
						GLenum *parameters, GLenum *values);
	void			assign(Texture &texture,
						GLenum target);
	void			load();
	void			generate_mipmap();
	void			set_pixel(const VEC2 &uv, const VEC4 &value);
	void			blur(const int &pass, const float &radius);
	GLenum			target() const;
	void			format(GLenum *format,
						GLenum *internal_format);
	GLuint			glid() const;
	UCHAR			*data() const;
	UCHAR			bpp() const;
	VEC2			size() const;
	VEC4			texelfetch(const VEC2 &uv);
	VEC4			sample(const VEC2 &uv);
	const std::string	&name();
	const std::string	&name() const;
protected :
	ULL			_id;
	GLuint		_glid;
	std::string	_name;
	VEC2		_size;
	char		_bpp;
	GLenum		_target;
	GLenum		_format;
	GLenum		_internal_format;
	UCHAR		*_data;
	bool		_loaded;
	Texture(const std::string &name);
};

struct	BMP : public Texture
{
	static Texture	*parse(const std::string &texture_name, const std::string &imagepath);
	static void		save(const Texture &, const std::string &);
};

class	Framebuffer : public Texture
{
public :
	static Framebuffer	&create(const std::string &name, VEC2 size, Shader &shader,
				int color_attachements, int depth);
	static void	bind_default();
	void		bind(bool to_bind = true);
	Texture		&attachement(int color_attachement);
	Texture		&depth();
	Shader		&shader();
	void	setup_attachements();
	Texture		&create_attachement(GLenum format, GLenum iformat);
	void	destroy(void *buffer);
	void	resize(const VEC2 &new_size);
private :
	Framebuffer(const std::string &name);
	void		_resize_depth(const VEC2 &);
	void		_resize_attachement(const int &, const VEC2 &);
	void		resize_attachement(const int &, const VEC2 &);
	std::vector<Texture*>	_color_attachements;
	Texture		*_depth;
	Shader		*_shader;
};

class Node
{
public:
	static Node &create(const std::string &name, VEC3 position, VEC3 rotation, VEC3 scale);
	static Node	&get_by_name(const std::string &);
	virtual void	physics_update();
	virtual void	fixed_update() {};
	virtual void	update() {};
	virtual void	render() {};
	MAT4		&mat4_transform();
	MAT4		&mat4_translation();
	MAT4		&mat4_rotation();
	MAT4		&mat4_scale();
	VEC3		&position();
	VEC3		&rotation();
	VEC3		&scale();
	VEC3		&up();
	void		add_child(Node &child);
	void		set_parent(Node &parent);
	void		set_name(const std::string &);
	const std::string	&name();
	Node		*parent;
	std::vector<Node *> children;
	BoundingElement		bounding_element;
protected :
	ULL			_id;
	std::string	_name;
	t_transform	_transform;
	Node(const std::string &name);
};

struct	Light : public Node
{
	int8_t		type;
	int8_t		cast_shadow;
	Framebuffer		*render_buffer;
};

struct PointLight : public Light
{
	VEC3		color;
	float		power;
	float		attenuation;
	float		falloff;
};

struct DirectionnalLight : public Light
{
	VEC3		color;
	float		power;
};

struct	Material
{
	static Material *create(const std::string &);
	static Material	*get_by_name(const std::string &);
	virtual void	bind_values() {};
	virtual void	bind_textures() {};
	virtual void	load_textures() {};
	void			set_name(const std::string &);
	const std::string		&name();
	Shader			*shader;
protected :
	std::string		_name;
	ULL				_id;
	Material(const std::string &name);
};

struct	PBRMaterial : public Material
{
	static PBRMaterial *create(const std::string &);
	void	bind_values();
	void	bind_textures();
	void	load_textures();
	VEC3		albedo;
	VEC3		specular;
	VEC3		emitting;
	VEC2		uv_scale;
	float		roughness;
	float		metallic;
	float		alpha;
	float		parallax;
	Texture		*texture_albedo;
	Texture		*texture_specular;
	Texture		*texture_roughness;
	Texture		*texture_metallic;
	Texture		*texture_emitting;
	Texture		*texture_normal;
	Texture		*texture_height;
	Texture		*texture_ao;
private :
	static Texture	*_texture_brdf;
	PBRMaterial(const std::string &name);
};

struct	Camera : public Node
{
	static Camera	&create(const std::string &, float fov);
	void			orbite(float phi, float theta, float radius);
	void			update();
	MAT4		view;
	MAT4		projection;
	FRUSTUM		frustum;
	float		fov;
	Node		*target;
private :
	Camera(const std::string &name);
};

# define CVEC4 struct s_charvec4

typedef struct s_charvec4
{
	UCHAR		x;
	UCHAR		y;
	UCHAR		z;
	UCHAR		w;
}				t_charvec4;

struct	Mesh : public Node
{
	static Mesh	*create(const std::string &);
	Material	*material;
	AABB		bounding_element;
	std::vector<VEC3>	v;
	std::vector<CVEC4>	vn;
	std::vector<VEC2>	vt;
	VEC2		uvmax;
	VEC2		uvmin;
	GLuint		v_arrayid;
	GLuint		v_bufferid;
	GLuint		vn_bufferid;
	GLuint		vt_bufferid;
	void		load();
	void		bind();
	void		render();
	void		center();
	Mesh		*parent;
	bool		is_loaded();
private :
	bool		_is_loaded;
	Mesh(const std::string &name);
};

class Events
{
public:
	static int	filter(void *e, SDL_Event *event);
	static int	refresh(void);
	static void	window(SDL_Event *event);
	static void	keyboard(SDL_Event *event);
	static void	check_events();
	static void	set_key_callback(SDL_Scancode keycode, t_kcallback callback);
	static void	set_refresh_callback(t_kcallback callback);
private :
	static Events	&_get();
	t_kcallback	_kcallbacks[285];
	t_kcallback	_rcallback;
	Events();
};

class	Window
{
public :
	static void		resize();
	static VEC2		size();
	static void		init(const std::string &name, int width, int height);
	static void		fullscreen(const bool &fullscreen);
	static void		swap();
	static GLbitfield	&clear_mask();
	static VEC4			&clear_color();
	static Framebuffer	&render_buffer();
private :
	static Window	&_get();
	SDL_Window	*_sdl_window;
	GLCONTEXT	_gl_context;
	VEC4		_clear_color;
	GLbitfield	_clear_mask;
	Framebuffer		*_render_buffer;
	Window() {};
};

class Cubemap : public Texture
{
public:
	Texture	*sides[6];
};

class Environment : public Cubemap
{
public:
	Environment();
	~Environment();
	Cubemap	*diffuse;
	Cubemap *brdf;
};

class	Engine
{
public :
	~Engine();
	static void		init(std::string &program_path);
	static float	delta_time();
	static float	fixed_delta_time();
	static void		run();
	static void		stop() { _get()._loop = false; };
	static void		add(Mesh &);
	static void		add(Camera &);
	static void		add(Light &);
	static void		add(Node &);
	static void		add(Material &);
	static void		add(Shader &);
	static void		add(Texture &);
	static void		add(Framebuffer &);
	static float	&internal_quality();
	static int8_t	&swap_interval();
	static void		set_current_camera(Camera &camera);
	static Camera	*current_camera();
	static Camera	*camera(const unsigned &);
	static Light	*light(const unsigned &);
	static Mesh		*mesh(const unsigned &);
	static Node		*node(const unsigned &);
	static Material	*material(const unsigned &);
	static Shader	*shader(const unsigned &);
	static Texture	*texture(const unsigned &);
	static Texture	*texture_env(const unsigned &);
	static void		fixed_update();
	static void		update();
	static std::string	program_path();
	static std::string	execution_path();
private :
	Engine();
	static Engine				&_get();
	void						_set_program_path(std::string &argv0);
	void						_setup_sdl(void);
	void						_load_env();
	bool						_loop;
	int8_t						_swap_interval;
	short						_env;
	short						_env_brdf;
	float						_delta_time;
	std::string					_program_path;
	std::string					_exec_path;
	float						_internal_quality;
	Camera						*_current_camera;
	std::vector<Node *>			_nodes;
	std::vector<Mesh *>			_meshes;
	std::vector<Camera *>		_cameras;
	std::vector<Light *>		_lights;
	std::vector<Material *>		_materials;
	std::vector<Shader *>		_shaders;
	std::vector<Texture *>		_textures;
	std::vector<Cubemap *>		_textures_env;
	std::vector<Framebuffer *>	_framebuffers;
};

/*
** Cubemap functions
*/
Texture			&cubemap_load(const std::string &name, const std::string &path);

/*
** Parser tools
*/

std::string		convert_backslash(std::string str);

/*
** .mtllib parser
*/
bool			load_mtllib(const std::string &path);

/*
** .obj parser
*/

Mesh			*load_obj(const std::string &path);

/*
** Callback functions
*/

void			callback_refresh(SDL_Event *event);
//void			callback_scale(SDL_Event *event);
//void			callback_stupidity(SDL_Event *event);
//void			callback_background(SDL_Event *event);
void			callback_exit(SDL_Event *event);
void			callback_fullscreen(SDL_Event *event);
void			callback_camera(SDL_Event *event);
void			callback_quality(SDL_Event *event);

/*
** render functions
*/

//void			render_shadow();
void			render_present();
void			render_scene();
GLuint			display_quad_get();

#endif
