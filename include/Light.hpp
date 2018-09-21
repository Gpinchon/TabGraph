/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 14:48:59 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"
#include "TextureArray.hpp"

class Framebuffer;

enum LightType
{
	Point, Directionnal
};

class	Light : public Node
{
public :
	static std::shared_ptr<Light>	create(const std::string &name, VEC3 color, VEC3 position, float power);
	static std::shared_ptr<Light>	get_by_name(const std::string &);
	static std::shared_ptr<Light>	get(unsigned index);
	virtual void					render_shadow();
	VEC3							&color();
	float							&power();
	bool							&cast_shadow();
	std::shared_ptr<Framebuffer>	render_buffer();
	//static TextureArray				*shadow_array();
	virtual LightType				type();
protected :
	static std::vector<std::shared_ptr<Light>>	_lights;
	Light(const std::string &name);
	VEC3						_color{0, 0, 0};
	float						_power{0};
	bool						_cast_shadow{false};
	std::weak_ptr<Framebuffer>	_render_buffer;
};

class DirectionnalLight : public Light
{
public :
	static std::shared_ptr<DirectionnalLight>	create(const std::string &name, VEC3 color, VEC3 position, float power, bool cast_shadow = false);
	virtual void								render_shadow();
	virtual void								fixed_update();
	virtual LightType							type();
protected :
	DirectionnalLight(const std::string &name);
};
