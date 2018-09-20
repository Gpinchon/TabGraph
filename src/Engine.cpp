/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Engine.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/02/07 18:23:47 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/20 19:11:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/InternalTools.hpp"
#include "Environment.hpp"
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

void			CFG::Load()
{
	auto	fd = fopen((Engine::program_path() + "config.ini").c_str(), "r");
	char	buffer[4096];
	while (fgets(buffer, 4096, fd))
	{
		unsigned	valu, valu1;
		int			vali;
		char		vals[4096];
		if (sscanf(buffer, "WindowName = %s", vals)) {
			CFG::WindowName() = vals;
		}
		else if (sscanf(buffer, "WindowSize = %u %u", &valu, &valu1)) {
			CFG::WindowSize().x = valu;
			CFG::WindowSize().y = valu1;
		}
		else if (sscanf(buffer, "MaxTexRes = %u", &vali)) {
			CFG::MaxTexRes() = vali;
		}
		else if (sscanf(buffer, "ShadowRes = %u", &valu)) {
			CFG::ShadowRes() = valu;
		}
		else if (sscanf(buffer, "Anisotropy = %u", &valu)) {
			CFG::Anisotropy() = valu;
		}
		else if (sscanf(buffer, "MSAA = %u", &valu)) {
			CFG::Msaa() = valu;
		}
		else if (sscanf(buffer, "BloomPass = %u", &valu)) {
			CFG::BloomPass() = valu;
		}
		else if (sscanf(buffer, "LightsPerPass = %u", &valu)) {
			CFG::LightsPerPass() = valu;
		}
		else if (sscanf(buffer, "ShadowsPerPass = %u", &valu)) {
			CFG::ShadowsPerPass() = valu;
		}
	}
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

int16_t		&CFG::MaxTexRes() {
	return (_get()->_maxTexRes);
}

uint16_t		&CFG::ShadowRes() {
	return (_get()->_shadowRes);
}

uint16_t		&CFG::Msaa() {
	return (_get()->_msaa);
}

uint16_t		&CFG::BloomPass() {
	return (_get()->_bloomPass);
}

uint16_t		&CFG::LightsPerPass() {
	return (_get()->_lightsPerPass);
}

uint16_t		&CFG::ShadowsPerPass() {
	return (_get()->_shadowsPerPass);
}

Engine::Engine() :
	_loop(),
	_frame_nbr(0),
	_swap_interval(1),
	_delta_time(0),
	_internal_quality(1)
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
		auto	newEnv = Environment::create(name);
		newEnv->set_diffuse(Cubemap::create(name + "Cube", HDR::parse(name, folder + name + "/environment.hdr")));
		newEnv->set_irradiance(Cubemap::create(name + "CubeDiffuse", HDR::parse(name + "Diffuse", folder + name + "/diffuse.hdr")));
	}
	folder = Engine::program_path() + "res/skybox/";
	dir = opendir(folder.c_str());
	while ((e = readdir(dir)) != nullptr)
	{
		if (e->d_name[0] == '.') {
			continue ;
		}
		std::string	name = e->d_name;
		auto	newEnv = Environment::create(name);
		try {
			newEnv->set_diffuse(Cubemap::parse(name, folder));
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
			continue;
		}
		try {
			newEnv->set_irradiance(Cubemap::parse(name + "/light", folder));
		}
		catch (std::exception &e) {
			std::cout << e.what() << std::endl;
		}
	}
	closedir(dir);
}

void			Engine::init()
{
	CFG::Load();
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
	for (auto i = 0; Node::node(i); i++) {
		auto node = Node::node(i);
		node->update();
	}
}

void	Engine::fixed_update()
{
	for (auto i = 0; Node::node(i); i++) {
		auto node = Node::node(i);
		node->physics_update();
	}
	for (auto i = 0; Node::node(i); i++) {
		auto node = Node::node(i);
		node->fixed_update();
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

int32_t		Engine::frame_nbr()
{
	return (_get()._frame_nbr);
}
