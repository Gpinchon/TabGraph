/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 21:07:54 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

struct	Light : public Node
{
	int8_t		type;
	int8_t		cast_shadow;
	Framebuffer		*render_buffer;
};

struct PointLight : public Light
{
	VEC3		color;
	float		power;
	float		attenuation;
	float		falloff;
};

struct DirectionnalLight : public Light
{
	VEC3		color;
	float		power;
};
