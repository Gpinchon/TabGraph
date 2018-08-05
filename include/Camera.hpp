/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/05/15 20:25:51 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 13:14:31 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Node.hpp"

class	Camera : public Node
{
public :
	static Camera	*create(const std::string &, float fov);
	void			orbite(float phi, float theta, float radius);
	void			update();
	MAT4			view{{0}};
	MAT4			projection{{0}};
	FRUSTUM			frustum{0, 0, 0, 0};
	float			fov{};
	Node			*target{nullptr};
private :
	Camera(const std::string &name);
};