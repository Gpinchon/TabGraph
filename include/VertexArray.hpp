/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexArray.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 20:09:27 by gpinchon          #+#    #+#             */
/*   Updated: 2018/08/26 19:10:00 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "GLIncludes.hpp"
#include <vector>

class VertexBuffer
{
public:
	template <typename T>
	VertexBuffer(GLuint attrib, GLenum data_type, int size, const std::vector<T> &a);
	void	bind(bool tobind = true);
private:
	GLuint	_GLid{0};
	VertexBuffer();
};

class VertexArray
{
public:
	static VertexArray	*create(size_t vertex_nbr, GLenum GLDrawType = GL_TRIANGLES);
	void				bind(bool tobind = true) const;
	void				draw() const;
	GLuint				glid() const;
	template <typename T>
	VertexBuffer		*add_buffer(GLenum data_type, int size, const std::vector<T> &a);
private:
	VertexArray() = default;
	std::vector<VertexBuffer*>	_buffers;
	bool						_bound{false};
	size_t						_vertex_nbr{0};
	GLuint						_GLid{0};
	GLenum						_GLDrawType{GL_TRIANGLES};
};

template <typename T>
VertexBuffer::VertexBuffer(GLuint attrib, GLenum data_type, int size, const std::vector<T> &a)
{
	glGenBuffers(1, &_GLid);
	glBindBuffer(GL_ARRAY_BUFFER, _GLid);
	glBufferData(GL_ARRAY_BUFFER, a.size() * sizeof(T), &a[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(attrib);
	glVertexAttribPointer(attrib, size, data_type, GL_FALSE, 0, (void*)nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

template <typename T>
VertexBuffer	*VertexArray::add_buffer(GLenum data_type, int size, const std::vector<T> &a)
{
	if (!size || a.empty()) {
		return (nullptr);
	}
	bind();
	auto	buffer = new VertexBuffer(_buffers.size(), data_type, size, a);
	_buffers.push_back(buffer);
	bind(false);
	return (buffer);
}