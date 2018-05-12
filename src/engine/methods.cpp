/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   methods.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/10 01:01:42 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "scop.hpp"
#include "Renderable.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Cubemap.hpp"
#include "Framebuffer.hpp"
#include <dirent.h>
#include <unistd.h>
#include <algorithm>

#ifndef _getcwd
#define _getcwd getcwd
#endif //_getcwd

Engine	*g_engine = nullptr;

/*
** engine is a singleton
*/

Engine::Engine() :
	_loop(),
	_swap_interval(1),
	_delta_time(0),
	_internal_quality(1),
	_current_camera(nullptr),
	_environment(nullptr)
{

}

Engine::~Engine()
{

}

Engine	&Engine::_get(void)
{
	if (!g_engine)
		g_engine = new Engine();
	return (*g_engine);
}

Environment::Environment() : diffuse(nullptr), brdf(nullptr)
{

}

void		Engine::sort(renderable_compare compare)
{
	std::sort(_get()._renderables.begin(), _get()._renderables.end(), compare);
}

Environment		*Engine::current_environment(Environment *env)
{
	if (env)
		_get()._environment = env;
	return (_get()._environment);
}

void			Engine::_load_res(void)
{
	DIR				*dir;
	struct dirent	*e;
	std::string		folder;

	folder = _get().program_path() + "res/skybox/";
	dir = opendir(folder.c_str());
	while ((e = readdir(dir)))
	{
		if (e->d_name[0] == '.')
			continue;
		std::string	name = e->d_name;
		auto	newEnv = new Environment;
		auto	t = Cubemap::parse(name, folder);
		t->load();
		newEnv->diffuse = t;
		t = Cubemap::parse(name + "/light", folder);
		t->load();
		newEnv->brdf = t;
		_get()._environments.push_back(newEnv);
	}
	closedir(dir);
	_get()._environment = _get()._environments[0];
}
void		Engine::add(Node &v)
{
	_get()._nodes.push_back(&v);
}

void		Engine::add(Renderable &v)
{
	_get()._renderables.push_back(&v);
	_get().add(static_cast<Node &>(v));

}
void		Engine::add(Camera &v)
{
	_get()._cameras.push_back(&v);	
	_get().add(static_cast<Node &>(v));

}
void		Engine::add(Light &v)
{
	_get()._lights.push_back(&v);
	_get().add(static_cast<Node &>(v));

}
void		Engine::add(Material &v)
{
	_get()._materials.push_back(&v);
}

void		Engine::add(Shader &v)
{
	_get()._shaders.push_back(&v);
}

void		Engine::add(Texture &v)
{
	_get()._textures.push_back(&v);
}

void		Engine::add(Framebuffer &v)
{
	_get()._framebuffers.push_back(&v);
}

void			Engine::init(std::string &program_path)
{
	_get()._loop = true;
	_get()._swap_interval = 1;
	_get()._internal_quality = 0.5;
	_get()._exec_path = convert_backslash(_getcwd(nullptr, 4096)) + "/";
	_get()._program_path = convert_backslash(program_path);
	_get()._program_path = _get()._program_path.substr(0, _get()._program_path.find_last_of('/'));
	_get()._program_path += "/";
	_get()._load_res();
	//_get()._set_program_path(program_path);
}

float	Engine::delta_time()
{
	return (_get()._delta_time);
}

int8_t	&Engine::swap_interval()
{
	return (_get()._swap_interval);
}

std::string	Engine::program_path()
{
	return (_get()._program_path);
}

std::string	Engine::execution_path()
{
	return (_get()._exec_path);
}

int		event_filter(void *arg, SDL_Event *event)
{
	return (Events::filter(arg, event));
}

void	Engine::update(void)
{
	unsigned	node_index = 0;
	while (auto node = Engine::node(node_index))
	{
		node->update();
		node_index++;
	}
}

void	Engine::fixed_update(void)
{
	unsigned	node_index = 0;

	Window::resize();
	while (auto node = Engine::node(node_index))
	{
		node->physics_update();
		node_index++;
	}
	node_index = 0;
	while (auto node = Engine::node(node_index))
	{
		node->fixed_update();
		node_index++;
	}
}

void	Engine::run(void)
{
	float	ticks;
	float	last_ticks;
	float	fixed_timing;

	SDL_GL_SetSwapInterval(Engine::_get().swap_interval());
	fixed_timing = last_ticks = SDL_GetTicks() / 1000.f;
	SDL_SetEventFilter(event_filter, nullptr);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	while (_get()._loop)
	{
		ticks = SDL_GetTicks() / 1000.f;
		_get()._delta_time = ticks - last_ticks;
		last_ticks = ticks;
		SDL_PumpEvents();
		Events::refresh();
		if (ticks - fixed_timing >= 0.016)
		{
			fixed_timing = ticks;
			fixed_update();
		}
		update();
		glClear(Window::clear_mask());
		//render_shadow();
		Window::render_buffer().bind();
		render_scene();
		Window::render_buffer().attachement(1).blur(BLOOMPASS, 2.5);
		//Window::render_buffer().attachement(2).blur(1, 2.5);
		render_present();
		Window::swap();
	}
}

float	&Engine::internal_quality()
{
	return (_get()._internal_quality);
}

void	Engine::set_current_camera(Camera &camera)
{
	_get()._current_camera = &camera;
}

Camera		*Engine::current_camera()
{
	return (_get()._current_camera);
}

Camera		*Engine::camera(const unsigned &index)
{
	if (index >= _get()._cameras.size())
		return (nullptr);
	return (_get()._cameras[index]);
}

Light		*Engine::light(const unsigned &index)
{
	if (index >= _get()._lights.size())
		return (nullptr);
	return (_get()._lights[index]);
}

Renderable	*Engine::renderable(const unsigned &index)
{
	if (index >= _get()._renderables.size())
		return (nullptr);
	return (_get()._renderables[index]);
}

Node			*Engine::node(const unsigned &index)
{
	if (index >= _get()._nodes.size())
		return (nullptr);
	return (_get()._nodes[index]);
}

Material		*Engine::material(const unsigned &index)
{
	if (index >= _get()._materials.size())
		return (nullptr);
	return (_get()._materials[index]);
}

Shader		*Engine::shader(const unsigned &index)
{
	if (index >= _get()._shaders.size())
		return (nullptr);
	return (_get()._shaders[index]);
}

Texture		*Engine::texture(const unsigned &index)
{
	if (index >= _get()._textures.size())
		return (nullptr);
	return (_get()._textures[index]);
}

Environment	*Engine::environment(const unsigned &index)
{
	if (index >= _get()._environments.size())
		return (nullptr);
	return (_get()._environments[index]);
}
