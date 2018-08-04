/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexArray.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:14:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/05 00:59:40 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VertexArray.hpp"

VertexArray	*VertexArray::create(size_t vertex_nbr, GLenum GLDrawType)
{
	auto	vao = new VertexArray();
	vao->_vertex_nbr = vertex_nbr;
	vao->_GLDrawType = GLDrawType;
	glGenVertexArrays(1, &vao->_GLid);
	return (vao);
}

GLuint	VertexArray::glid() const
{
	return (_GLid);
}

void	VertexArray::draw() const
{
	bind();
	glDrawArrays(_GLDrawType, 0, _vertex_nbr);
	bind(false);
};

void	VertexArray::bind(bool tobind) const
{
	if (tobind) {
		glBindVertexArray(_GLid);
	}
	else {
		glBindVertexArray(0);
	}
};