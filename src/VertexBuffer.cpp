/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:07:39
*/

#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer(const std::vector<unsigned>& indices)
{
    glGenBuffers(1, &_GLid);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _GLid);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);
    glCheckError();
    _size = indices.size();
}

size_t VertexBuffer::size()
{
    return (_size);
}

GLuint VertexBuffer::glid()
{
    return (_GLid);
}