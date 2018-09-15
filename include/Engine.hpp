/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Engine.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 20:02:35 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/15 19:57:22 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "GLIncludes.hpp" 
# include <vector>
# include <string>

# ifndef M_PI
const auto M_PI				= 3.14159265359f;
# endif //M_PI
auto UP						= (VEC3){0, 1, 0};
#define GL_DEBUG

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
	~Environment() = default;
	void	unload();
	Cubemap	*diffuse;
	Cubemap *irradiance;
};

typedef bool (*renderable_compare)(Renderable *m, Renderable *m1);

class	Engine
{
public :
	~Engine();
	static void		init();
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
	static void		add(Environment &);
	static float	&internal_quality();
	static int8_t	&swap_interval();
	static void		set_current_camera(Camera *camera);
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
	static int32_t				frame_nbr();
private :
	Engine();
	static Engine				&_get();
	static Engine				*_instance;
	void						_set_program_path(std::string &argv0);
	void						_load_res();
	bool						_loop;
	int32_t						_frame_nbr;
	int8_t						_swap_interval;
	float						_delta_time;
	std::string					_program_path;
	std::string					_exec_path;
	float						_internal_quality;
	Camera						*_current_camera{nullptr};
	Environment					*_environment{nullptr};
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

class CFG
{
public :
	static void			Load();
	static VEC2			&WindowSize();
	static std::string	&WindowName();
	static float		&Anisotropy();
	static int32_t		&ShadowRes();
	static int32_t		&MaxTexRes();
	static int16_t		&Msaa();
	static int16_t		&BloomPass();
	static int16_t		&LightsPerPass();
	static int16_t		&ShadowsPerPass();
private :
	static CFG	*_get();
	static CFG	*_instance;
	VEC2		_windowSize{1280, 720};
	std::string	_windowName{""};
	float		_anisotropy{16.f};
	int32_t		_shadowRes{2048};
	int32_t		_maxTexRes{-1};
	int16_t		_msaa{0};
	int16_t		_bloomPass{1};
	int16_t		_lightsPerPass{32};
	int16_t		_shadowsPerPass{16};
	CFG() = default;
	~CFG() = default;
	
};

/*
** render functions
*/

//void			render_shadow();
GLuint			display_quad_get();
