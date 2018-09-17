/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/17 17:14:09 by gpinchon         ###   ########.fr       */
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
	static Light		*create(const std::string &name, VEC3 color, VEC3 position, float power);
	virtual void		render_shadow();
	VEC3				&color();
	float				&power();
	bool				&cast_shadow();
	Framebuffer			*render_buffer();
	static TextureArray	*shadow_array();
	virtual LightType	type();
protected :
	Light(const std::string &name);
	VEC3		_color{0, 0, 0};
	float		_power{0};
	bool		_cast_shadow{false};
	Framebuffer	*_render_buffer{nullptr};
private :
	static TextureArray	*_shadow_array;
};

class DirectionnalLight : public Light
{
public :
	static DirectionnalLight	*create(const std::string &name, VEC3 color, VEC3 position, float power, bool cast_shadow = false);
	virtual void				render_shadow();
	virtual void				fixed_update();
	virtual LightType			type();
protected :
	DirectionnalLight(const std::string &name);
};
