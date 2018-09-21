/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexArray.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 21:14:28 by gpinchon          #+#    #+#             */
/*   Updated: 2018/09/21 18:08:47 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "VertexArray.hpp"

std::vector<std::shared_ptr<VertexArray>>	VertexArray::_vertexArrays;

std::shared_ptr<VertexArray>	VertexArray::create(size_t vertex_nbr, GLenum GLDrawType)
{
	auto	vao = std::shared_ptr<VertexArray>(new VertexArray());
	vao->_vertex_nbr = vertex_nbr;
	vao->_GLDrawType = GLDrawType;
	glGenVertexArrays(1, &vao->_GLid);
	_vertexArrays.push_back(vao);
	return (vao);
}

VertexBuffer	*VertexArray::add_indices(const std::vector<unsigned> &indices)
{
	if (indices.empty()) {
		return (nullptr);
	}
	bind();
	_indices = new VertexBuffer(indices);
	_indexed = true;
	bind(false);
	return (_indices);
}

GLuint	VertexArray::glid() const
{
	return (_GLid);
}

void	VertexArray::draw() const
{
	bind();
	if (_indexed) {
		glDrawElements(_GLDrawType, _indices->size(), GL_UNSIGNED_INT, nullptr);
	}
	else
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