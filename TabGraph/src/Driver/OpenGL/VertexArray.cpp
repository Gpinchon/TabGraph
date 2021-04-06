/*
* @Author: gpinchon
* @Date:   2021-03-25 11:08:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-25 11:08:56
*/

#include "Driver/OpenGL/VertexArray.hpp"
#include "Driver/OpenGL/Buffer.hpp"
#include "Buffer/BufferAccessor.hpp"

#include <GL/glew.h>
#include <array>

static inline auto GLComponentType(BufferAccessor::ComponentType type)
{
    static std::array<GLenum, (size_t)BufferAccessor::ComponentType::MaxValue> s_compTypeLUT = {
        GL_BYTE, //Int8
        GL_UNSIGNED_BYTE, //Uint8
        GL_SHORT, //Int16
        GL_UNSIGNED_SHORT, //Uint16
        GL_UNSIGNED_INT, //Uint32
        GL_FLOAT //Float32
    };
    return s_compTypeLUT.at(int(type));
}

VertexArray::VertexArray()
{
	glCreateVertexArrays(1, &_handle);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &_handle);
}

uint32_t VertexArray::GetHandle()
{
    return _handle;
}

VertexArray& VertexArray::Bind()
{
	glBindVertexArray(GetHandle());
	return *this;
}

VertexArray& VertexArray::BindAccessor(std::shared_ptr<BufferAccessor> accessor, int index)
{
    if (accessor == nullptr) {
        glDisableVertexAttribArray(index);
        return *this;
    }
    auto bufferView(accessor->GetBufferView());
    auto byteOffset(accessor->GetByteOffset());
    auto compType{ GLComponentType(accessor->GetComponentType()) };
    bufferView->Load();
    glEnableVertexAttribArray(index);
    //bufferView->Bind();
    //glVertexAttribPointer(
    //    index,
    //    (uint8_t)accessor->GetType(),
    //    (GLenum)accessor->GetComponentType(),
    //    accessor->GetNormalized(),
    //    bufferView->GetByteStride() ? bufferView->GetByteStride() : accessor->GetTypeOctetsSize(),
    //    BUFFER_OFFSET(accessor->GetByteOffset())
    //);
    //bufferView->Done();
    glVertexAttribFormat(
        index,
        (uint8_t)accessor->GetType(),
        compType,
        accessor->GetNormalized(),
        0);
    glBindVertexBuffer(
        index,
        bufferView->GetHandle(),
        accessor->GetByteOffset(),
        bufferView->GetByteStride() ? bufferView->GetByteStride() : accessor->GetTypeOctetsSize());
    return *this;
}

void VertexArray::Done()
{
    BindNone();
}

void VertexArray::BindNone()
{
    glBindVertexArray(0);
}
