/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scop.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/02/18 20:44:18 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 22:31:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "GLIncludes.hpp" 
# include <stdint.h>
# include <vml.h>
# include <iostream>
# include <fcntl.h>
# include <stdio.h>
# include <vector>
# include <map>
# include <string>
# include <functional>

# ifndef M_PI
const auto M_PI = 3.14159265359;
# endif //M_PI
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
class Framebuffer;
class Cubemap;
class Renderable;
class Node;
class Camera;
class Light;

enum			e_rendertype
{
	render_all = 0, render_opaque = 1, render_transparent = 2
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

struct	Material
{
	static Material *create(const std::string &);
	static Material	*get_by_name(const std::string &);
	virtual void	bind_values();
	virtual void	bind_textures();
	virtual void	load_textures();
	void			set_name(const std::string &);
	const std::string		&name();
	Shader			*shader;
	VEC3			albedo;
	VEC3			emitting;
	VEC2			uv_scale;
	float			alpha;
	Texture			*texture_albedo;
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
	VEC3		specular;
	float		roughness;
	float		metallic;
	float		parallax;
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

class Environment
{
public:
	static Environment *parse(const std::string &, const std::string &);
	Environment();
	~Environment();
	Cubemap	*diffuse;
	Cubemap *brdf;
};

typedef bool (*renderable_compare)(Renderable *m, Renderable *m1);	

class	Engine
{
public :
	~Engine();
	static void		init(std::string &program_path);
	static float	delta_time();
	static float	fixed_delta_time();
	static void		run();
	static void		stop() { _get()._loop = false; };
	static void		add(Renderable &);
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
	static Environment	*current_environment(Environment *env = nullptr);
	static Camera	*camera(const unsigned &);
	static Light	*light(const unsigned &);
	static Renderable	*renderable(const unsigned &);
	static Node		*node(const unsigned &);
	static Material	*material(const unsigned &);
	static Shader	*shader(const unsigned &);
	static Texture	*texture(const unsigned &);
	static Environment	*environment(const unsigned &);
	static void		fixed_update();
	static void		update();
	static std::string	program_path();
	static std::string	execution_path();
	static void		sort(renderable_compare);
private :
	Engine();
	static Engine				&_get();
	void						_set_program_path(std::string &argv0);
	void						_load_res();
	bool						_loop;
	int8_t						_swap_interval;
	float						_delta_time;
	std::string					_program_path;
	std::string					_exec_path;
	float						_internal_quality;
	Camera						*_current_camera;
	Environment					*_environment;
	std::vector<Node *>			_nodes;
	std::vector<Renderable *>			_renderables;
	std::vector<Camera *>		_cameras;
	std::vector<Light *>		_lights;
	std::vector<Material *>		_materials;
	std::vector<Shader *>		_shaders;
	std::vector<Texture *>		_textures;
	std::vector<Environment *>	_environments;
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
** Callback functions
*/

void			callback_refresh(SDL_Event *event);
void			callback_scale(SDL_Event *event);
void			callback_background(SDL_Event *event);
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
