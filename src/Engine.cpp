/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Engine.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/14 17:30:58 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/InternalTools.hpp"
#include "Render.hpp"
#include "Camera.hpp"
#include "Cubemap.hpp"
#include "Engine.hpp"
#include "Events.hpp"
#include "Framebuffer.hpp"
#include "Light.hpp"
#include "Renderable.hpp"
#include "Window.hpp"
#include <algorithm>
#include <dirent.h>
#include <unistd.h>
#include "parser/HDR.hpp"

#ifndef _getcwd
#define _getcwd getcwd
#endif //_getcwd

Engine	*Engine::_instance = nullptr;
CFG		*CFG::_instance = nullptr;

/*
** engine is a singleton
*/

#include <iostream>

CFG			*CFG::_get()
{
	if (nullptr == _instance)
		_instance = new CFG();
	return (_instance);
}

VEC2		&CFG::WindowSize() {
	return (_get()->_windowSize);
}

std::string	&CFG::WindowName() {
	return (_get()->_windowName);
}

float		&CFG::Anisotropy() {
	return (_get()->_anisotropy);
}

int32_t		&CFG::ShadowRes() {
	return (_get()->_shadowRes);
}

int32_t		&CFG::MaxTexRes() {
	return (_get()->_maxTexRes);
}

int16_t		&CFG::Msaa() {
	return (_get()->_msaa);
}

int16_t		&CFG::BloomPass() {
	return (_get()->_bloomPass);
}

int16_t		&CFG::LightsPerPass() {
	return (_get()->_lightsPerPass);
}

Engine::Engine() :
	_loop(),
	_frame_nbr(0),
	_swap_interval(1),
	_delta_time(0),
	_internal_quality(1),
	_current_camera(nullptr),
	_environment(nullptr)
{
	_loop = true;
	_swap_interval = 1;
	_internal_quality = 0.5;
	_exec_path = convert_backslash(_getcwd(nullptr, 4096)) + "/";
	_program_path = convert_backslash(SDL_GetBasePath());
	_program_path = _program_path.substr(0, _program_path.find_last_of('/'));
	_program_path += "/";
}

Engine::~Engine()
= default;

Engine	&Engine::_get()
{
	if (_instance == nullptr)
		_instance = new Engine();
	return (*_instance);
}

Environment::Environment() : diffuse(nullptr), irradiance(nullptr)
{

}

void	Environment::unload()
{
	diffuse->unload();
	irradiance->unload();
}

void		Engine::sort(renderable_compare compare)
{
	std::sort(_get()._renderables.begin(), _get()._renderables.end(), compare);
}

Environment		*Engine::current_environment(Environment *env)
{
	if (env != nullptr) {
		if (_get()._environment != nullptr)
			_get()._environment->unload();
		_get()._environment = env;
	}
	return (_get()._environment);
}

void			Engine::_load_cfg()
{
	auto	fd = fopen((Engine::program_path() + "config.ini").c_str(), "r");
	char	buffer[4096];
	while (fgets(buffer, 4096, fd))
	{
		int		val0, val1;
		char	vals[4096];
		if (sscanf(buffer, "WindowSize = %i %i", &val0, &val1)) {
			CFG::WindowSize().x = val0;
			CFG::WindowSize().y = val1;
		}
		else if (sscanf(buffer, "WindowName = %s", vals)) {
			CFG::WindowName() = vals;
		}
		else if (sscanf(buffer, "ShadowRes = %i", &val0)) {
			CFG::ShadowRes() = val0;
		}
		else if (sscanf(buffer, "Anisotropy = %i", &val0)) {
			CFG::Anisotropy() = val0;
		}
		else if (sscanf(buffer, "MSAA = %i", &val0)) {
			CFG::Msaa() = val0;
		}
		else if (sscanf(buffer, "BloomPass = %i", &val0)) {
			CFG::BloomPass() = val0;
		}
		else if (sscanf(buffer, "MaxTexRes = %i", &val0)) {
			CFG::MaxTexRes() = val0;
		}
		else if (sscanf(buffer, "LightsPerPass = %i", &val0)) {
			CFG::LightsPerPass() = val0;
		}
	}
}

void			Engine::_load_res()
{
	DIR				*dir;
	struct dirent	*e;
	std::string		folder;

	folder = Engine::program_path() + "res/hdr/";
	dir = opendir(folder.c_str());
	while (dir != nullptr && (e = readdir(dir)) != nullptr)
	{
		if (e->d_name[0] == '.') {
			continue ;
		}
		std::string	name = e->d_name;
		auto	newEnv = new Environment;
		newEnv->diffuse = Cubemap::create(name + "Cube", HDR::parse(name, folder + name + "/environment.hdr"));
		newEnv->irradiance = Cubemap::create(name + "CubeDiffuse", HDR::parse(name + "Diffuse", folder + name + "/diffuse.hdr"));
		Engine::add(*newEnv);
	}
	folder = Engine::program_path() + "res/skybox/";
	dir = opendir(folder.c_str());
	while ((e = readdir(dir)) != nullptr)
	{
		if (e->d_name[0] == '.') {
			continue ;
		}
		std::string	name = e->d_name;
		auto	newEnv = new Environment;
		try {
			newEnv->diffuse = Cubemap::parse(name, folder);
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
			delete newEnv;
			continue;
		}
		try {
			newEnv->irradiance = Cubemap::parse(name + "/light", folder);
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
		Engine::add(*newEnv);
	}
	closedir(dir);
	Engine::current_environment(Engine::environment(0));
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

void		Engine::add(Environment &v)
{
	_get()._environments.push_back(&v);
}

void			Engine::init()
{
	_get()._load_cfg();
	Window::init(CFG::WindowName(), CFG::WindowSize().x, CFG::WindowSize().y);
	_get()._load_res();
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

void	Engine::update()
{
	unsigned	node_index = 0;

	while (auto node = Engine::node(node_index))
	{
		node->update();
		node_index++;
	}
}

void	Engine::fixed_update()
{
	unsigned	node_index = 0;

	node_index = 0;
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

void	Engine::run()
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
		_get()._frame_nbr++;
		_get()._delta_time = ticks - last_ticks;
		last_ticks = ticks;
		SDL_PumpEvents();
		Events::refresh();
		if (ticks - fixed_timing >= 0.015)
		{
			fixed_timing = ticks;
			Engine::fixed_update();
			Render::fixed_update();
		}
		Engine::update();
		Render::update();
		Render::scene();
	}
}

float	&Engine::internal_quality()
{
	return (_get()._internal_quality);
}

void	Engine::set_current_camera(Camera *camera)
{
	_get()._current_camera = camera;
}

Camera		*Engine::current_camera()
{
	return (_get()._current_camera);
}

Camera		*Engine::camera(const unsigned &index)
{
	if (index >= _get()._cameras.size()) {
		return (nullptr);
	}
	return (_get()._cameras[index]);
}

Light		*Engine::light(const unsigned &index)
{
	if (index >= _get()._lights.size()) {
		return (nullptr);
	}
	return (_get()._lights[index]);
}

Renderable	*Engine::renderable(const unsigned &index)
{
	if (index >= _get()._renderables.size()) {
		return (nullptr);
	}
	return (_get()._renderables[index]);
}

Node			*Engine::node(const unsigned &index)
{
	if (index >= _get()._nodes.size()) {
		return (nullptr);
	}
	return (_get()._nodes[index]);
}

Material		*Engine::material(const unsigned &index)
{
	if (index >= _get()._materials.size()) {
		return (nullptr);
	}
	return (_get()._materials[index]);
}

Shader		*Engine::shader(const unsigned &index)
{
	if (index >= _get()._shaders.size()) {
		return (nullptr);
	}
	return (_get()._shaders[index]);
}

Texture		*Engine::texture(const unsigned &index)
{
	if (index >= _get()._textures.size()) {
		return (nullptr);
	}
	return (_get()._textures[index]);
}

Environment	*Engine::environment(const unsigned &index)
{
	if (index >= _get()._environments.size()) {
		return (nullptr);
	}
	return (_get()._environments[index]);
}

int32_t		Engine::frame_nbr()
{
	return (_get()._frame_nbr);
}
