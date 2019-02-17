/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexBuffer.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/17 19:36:53 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 19:40:17 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(const std::vector<unsigned> &indices)
{
	glGenBuffers(1, &_GLid);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _GLid);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
	glCheckError();
	_size = indices.size();
}

size_t	VertexBuffer::size()
{
	return (_size);
}

GLuint	VertexBuffer::glid()
{
	return (_GLid);
}