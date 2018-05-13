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
const auto M_PI =		3.14159265359;
# endif //M_PI
const auto WIDTH =		1280;
const auto HEIGHT =		720;
const auto SHADOWRES =	2048;
const auto UP =			(VEC3){0, 1, 0};
const auto ANISOTROPY =	4.f;
const auto MSAA =		4;
const auto BLOOMPASS =	2;
const auto MAXTEXRES =	1024;
# ifndef O_BINARY
#  define O_BINARY 0x0004
# endif
# define RENDERTYPE	enum e_rendertype
# define GLCONTEXT	SDL_GLContext

class Material;
class Shader;
class Texture;
class Framebuffer;
class Cubemap;
class Renderable;
class Node;
class Camera;
class Light;

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
	std::vector<Renderable *>	_renderables;
	std::vector<Camera *>		_cameras;
	std::vector<Light *>		_lights;
	std::vector<Material *>		_materials;
	std::vector<Shader *>		_shaders;
	std::vector<Texture *>		_textures;
	std::vector<Environment *>	_environments;
	std::vector<Framebuffer *>	_framebuffers;
};

/*
** Key binding
*/

# define DOWNK		SDL_SCANCODE_DOWN
# define UPK		SDL_SCANCODE_UP
# define LEFTK		SDL_SCANCODE_LEFT
# define RIGHTK		SDL_SCANCODE_RIGHT
# define ZOOMK		SDL_SCANCODE_KP_PLUS
# define UNZOOMK	SDL_SCANCODE_KP_MINUS

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
