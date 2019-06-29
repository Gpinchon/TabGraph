/*
* @Author: gpi
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpi
* @Last Modified time: 2019-05-06 14:07:34
*/

#include "VertexArray.hpp"
#include "Debug.hpp"

std::vector<std::shared_ptr<VertexArray>> VertexArray::_vertexArrays;

std::shared_ptr<VertexArray> VertexArray::create(size_t vertex_nbr, GLenum GLDrawType)
{
    auto vao = std::shared_ptr<VertexArray>(new VertexArray());
    vao->_vertex_nbr = vertex_nbr;
    vao->_GLDrawType = GLDrawType;
    glGenVertexArrays(1, &vao->_GLid);
    glCheckError();
    _vertexArrays.push_back(vao);
    return (vao);
}

VertexBuffer* VertexArray::add_indices(const std::vector<unsigned>& indices)
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

void VertexArray::set_draw_type(GLenum draw_type)
{
    _GLDrawType = draw_type;
}

GLuint VertexArray::glid() const
{
    return (_GLid);
}

void VertexArray::draw() const
{
    bind();
    if (_indexed) {
        glDrawElements(_GLDrawType, _indices->size(), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(_GLDrawType, 0, _vertex_nbr);
    }
    glCheckError();
    bind(false);
};

void VertexArray::bind(bool tobind) const
{
    if (tobind) {
        glBindVertexArray(_GLid);
    } else {
        glBindVertexArray(0);
    }
    glCheckError();
};