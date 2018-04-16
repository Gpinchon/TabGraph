/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quad.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/04/13 16:32:22 by gpinchon          #+#    #+#             */
/*   Updated: 2018/04/16 18:20:41 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <scop.h>

/*
** quad is a singleton
*/

inline GLuint	display_quad_get(void)
{
	static GLuint	render_quadid = 0;
	GLuint			bufferid;
	float			quad[8];

	if (render_quadid)
		return (render_quadid);
	quad[0] = -1.0f;
	quad[1] = -1.0f;
	quad[2] = 1.0f;
	quad[3] = -1.0f;
	quad[4] = -1.0f;
	quad[5] = 1.0f;
	quad[6] = 1.0f;
	quad[7] = 1.0f;
	glGenVertexArrays(1, &render_quadid);
	glBindVertexArray(render_quadid);
	glGenBuffers(1, &bufferid);
	glBindBuffer(GL_ARRAY_BUFFER, bufferid);
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), quad, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return (render_quadid);
}
