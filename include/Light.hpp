/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/19 23:00:20 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

class	Light : public Node
{
public :
	int8_t			type;
	int8_t			cast_shadow;
	Framebuffer		*render_buffer{nullptr};
};

class PointLight : public Light
{
public :
	VEC3		color;
	float		power;
	float		attenuation;
	float		falloff;
};

class DirectionnalLight : public Light
{
public :
	VEC3		color;
	float		power;
};
