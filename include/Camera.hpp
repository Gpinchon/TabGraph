/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/15 21:08:11 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"
#include "vml.h"

struct	Camera : public Node
{
	static Camera	*create(const std::string &, float fov);
	void			orbite(float phi, float theta, float radius);
	void			update();
	MAT4		view;
	MAT4		projection;
	FRUSTUM		frustum;
	float		fov;
	Node		*target;
private :
	Camera(const std::string &name);
};