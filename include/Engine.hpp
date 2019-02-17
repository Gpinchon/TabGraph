/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Engine.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/13 20:02:35 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 22:45:25 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include "GLIncludes.hpp"
# include "Object.hpp"
# include <memory>
# include <vector>
# include <string>

# ifndef M_PI
#define M_PI				3.14159265359f
# endif //M_PI
#define UP					(VEC3){0, 1, 0}
//#define DEBUG_MOD

class Material;
class Shader;
class Texture;
class Framebuffer;
class Cubemap;
class Renderable;
class Node;
class Camera;
class Light;

class	Engine
{
public :
	~Engine();
	static void		init();
	static float	delta_time();
	static float	fixed_delta_time();
	static void		run();
	static void		stop() { _get()._loop = false; };
	static float	&internal_quality();
	static int8_t	&swap_interval();
	static void		fixed_update();
	static void		update();
	static int32_t	frame_nbr();
	static std::string			&execution_path();
	static std::string			&program_path();
	static const std::string	&resource_path();
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
};
