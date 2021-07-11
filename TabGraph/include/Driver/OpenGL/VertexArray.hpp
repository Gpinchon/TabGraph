/*
* @Author: gpinchon
* @Date:   2021-03-25 11:09:05
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-25 11:14:50
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Driver/OpenGL/ObjectHandle.hpp>

#include <memory>
#include <GL/glew.h>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
//namespace TabGraph::Buffer {
//template<typename T>
//class Accessor;
//}

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace OpenGL {
class VertexArray {
public:
    using Handle = OpenGL::ObjectHandle;
    VertexArray();
    ~VertexArray();
    auto GetHandle() const
    {
        return _handle;
    }
    VertexArray& Bind();
    template<typename T>
    VertexArray& BindAccessor(const TabGraph::Buffer::Accessor<T>& accessor, int index);
    void Done();
    static void BindNone();

private:
    Handle _handle { 0 };
};
template<typename T>
static inline auto GetTypeValuesNbr() {
    static_assert("type not managed");
    return 0;
}
template<>
static inline auto GetTypeValuesNbr<float>() {
    return 1;
}
template<>
static inline auto GetTypeValuesNbr<unsigned>() {
    return 1;
}
template<>
static inline auto GetTypeValuesNbr<glm::vec2>() {
    return 2;
}
template<>
static inline auto GetTypeValuesNbr<glm::vec3>() {
    return 3;
}
template<>
static inline auto GetTypeValuesNbr<glm::vec4>() {
    return 4;
}

template<typename T>
static inline auto GetOpenGLType() {
    static_assert("type not managed");
    return GL_NONE;
}
template<>
static inline auto GetOpenGLType<float>() {
    return GL_FLOAT;
}
template<>
static inline auto GetOpenGLType<unsigned>() {
    return GL_UNSIGNED_INT;
}
template<>
static inline auto GetOpenGLType<glm::vec2>() {
    return GL_FLOAT;
}
template<>
static inline auto GetOpenGLType<glm::vec3>() {
    return GL_FLOAT;
}
template<>
static inline auto GetOpenGLType<glm::vec4>() {
    return GL_FLOAT;
}

template<typename T>
VertexArray& VertexArray::BindAccessor(const TabGraph::Buffer::Accessor<T>& accessor, int index)
{
    auto bufferView(accessor.GetBufferView());
    if (bufferView == nullptr) {
        glDisableVertexAttribArray(index);
        return *this;
    }
    auto byteOffset(accessor.GetByteOffset());
    bufferView->Load();
    glEnableVertexAttribArray(index);
    glVertexAttribFormat(
        index,
        GetTypeValuesNbr<T>(),
        GetOpenGLType<T>(),
        accessor.GetNormalized(),
        0);
    glBindVertexBuffer(
        index,
        OpenGL::GetHandle(bufferView),
        accessor.GetByteOffset(),
        bufferView->GetByteStride() ? bufferView->GetByteStride() : sizeof(T));
    return *this;
}
}
