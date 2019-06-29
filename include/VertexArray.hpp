/*
* @Author: gpi
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpi
* @Last Modified time: 2019-06-24 16:39:35
*/

#pragma once

#include <GL/glew.h>         // for GLenum, GL_TRIANGLES, GLuint
#include <stddef.h>          // for size_t
#include <memory>            // for shared_ptr
#include <vector>            // for vector
#include "VertexBuffer.hpp"  // for VertexBuffer

class VertexArray {
public:
    static std::shared_ptr<VertexArray> create(size_t vertex_nbr, GLenum GLDrawType = GL_TRIANGLES);
    void bind(bool tobind = true) const;
    void draw() const;
    GLuint glid() const;
    template <typename T>
    VertexBuffer* add_buffer(GLenum data_type, int size, const std::vector<T>& a);
    VertexBuffer* add_indices(const std::vector<unsigned int>& a);
    void set_draw_type(GLenum);

private:
    static std::vector<std::shared_ptr<VertexArray>> _vertexArrays;
    std::vector<VertexBuffer*> _buffers;
    VertexBuffer* _indices;
    bool _indexed{ false };
    size_t _vertex_nbr{ 0 };
    GLuint _GLid{ 0 };
    GLenum _GLDrawType{ GL_TRIANGLES };
    VertexArray() = default;
};

template <typename T>
VertexBuffer* VertexArray::add_buffer(GLenum data_type, int size, const std::vector<T>& a)
{
    if (!size || a.empty()) {
        return (nullptr);
    }
    bind();
    auto buffer = new VertexBuffer(_buffers.size(), data_type, size, a);
    _buffers.push_back(buffer);
    bind(false);
    return (buffer);
}
