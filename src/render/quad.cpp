/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 16:32:22 by gpinchon          #+#    #+#             */
/*   Updated: 2018/05/20 17:35:33 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "GLIncludes.hpp"
#include <array>

/*
** quad is a singleton
*/

GLuint	display_quad_get()
{
	static GLuint	render_quadid = 0;
	GLuint			bufferid;
	std::array<float, 8>	quad;

	if (render_quadid != 0u) {
		return (render_quadid);
	}
	quad.at(0) = -1.0f;
	quad.at(1) = -1.0f;
	quad.at(2) = 1.0f;
	quad.at(3) = -1.0f;
	quad.at(4) = -1.0f;
	quad.at(5) = 1.0f;
	quad.at(6) = 1.0f;
	quad.at(7) = 1.0f;
	glGenVertexArrays(1, &render_quadid);
	glBindVertexArray(render_quadid);
	glGenBuffers(1, &bufferid);
	glBindBuffer(GL_ARRAY_BUFFER, bufferid);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &quad.at(0), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return (render_quadid);
}
