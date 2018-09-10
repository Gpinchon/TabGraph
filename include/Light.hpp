/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/10 19:40:22 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

class	Light : public Node
{
public :
	VEC3		color;
	float		power;
	int8_t			type;
	int8_t			cast_shadow;
	Framebuffer		*render_buffer{nullptr};
};

class PointLight : public Light
{
public :
	float		attenuation;
	float		falloff;
};

class DirectionnalLight : public Light
{
public :
	float		power;
};
