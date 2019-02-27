/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexBuffer.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/02/17 19:38:15 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 19:44:11 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Debug.hpp"
#include "GLIncludes.hpp"
#include <vector>

class VertexBuffer {
public:
    template <typename T>
    VertexBuffer(GLuint attrib, GLenum data_type, int size, const std::vector<T>& a);
    VertexBuffer(const std::vector<unsigned int>& a);
    GLuint glid();
    void bind(bool tobind = true);
    size_t size();

private:
    size_t _size{ 0 };
    GLuint _GLid{ 0 };
    VertexBuffer();
};

template <typename T>
VertexBuffer::VertexBuffer(GLuint attrib, GLenum data_type, int size, const std::vector<T>& a)
{
    glGenBuffers(1, &_GLid);
    glBindBuffer(GL_ARRAY_BUFFER, _GLid);
    glBufferData(GL_ARRAY_BUFFER, a.size() * sizeof(T), &a[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(attrib);
    glVertexAttribPointer(attrib, size, data_type, GL_FALSE, 0, (void*)nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glCheckError();
    _size = a.size();
}