/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Render.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 19:30:14 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 00:55:18 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class VertexArray;

namespace Render
{
	void		scene();
	void		present();
	void		bind_textures();
	const VertexArray	*display_quad();
};