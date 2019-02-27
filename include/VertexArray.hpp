/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   VertexArray.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpinchon <gpinchon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/08/04 20:09:27 by gpinchon          #+#    #+#             */
/*   Updated: 2019/02/17 19:39:53 by gpinchon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Debug.hpp"
#include "GLIncludes.hpp"
#include "VertexBuffer.hpp"
#include <memory>
#include <vector>

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
