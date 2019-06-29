/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:29:25
*/

#pragma once

#include <GL/glew.h>  // for GLuint, GL_ARRAY_BUFFER, GLenum, glBindBuffer
#include <stddef.h>   // for size_t
#include <vector>     // for vector
#include "Debug.hpp"  // for glCheckError

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