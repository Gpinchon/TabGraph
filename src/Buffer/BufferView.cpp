/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 17:47:00
*/
#include "Buffer/BufferView.hpp"
#include "Buffer/Buffer.hpp"

BufferView::BufferView(size_t byteLength, std::shared_ptr<Buffer> buffer)
    : Component("")
    , _byteLength(byteLength)
{
    SetComponent(buffer);
}

std::shared_ptr<BufferView> BufferView::Create(size_t byteLength, std::shared_ptr<Buffer> buffer)
{
    return std::shared_ptr<BufferView>(new BufferView(byteLength, buffer));
}

/** The buffer. */
std::shared_ptr<Buffer> BufferView::GetBuffer()
{
    return GetComponent<Buffer>();
}

void BufferView::SetBuffer(std::shared_ptr<Buffer> buffer)
{
    SetComponent(buffer);
}

size_t BufferView::ByteOffset() const
{
    return _byteOffset;
}

void BufferView::SetByteOffset(size_t byteOffset)
{
    _byteOffset = byteOffset;
}

size_t BufferView::ByteLength() const
{
    return _byteLength;
}

void BufferView::SetByteLength(size_t byteLength)
{
    _byteLength = byteLength;
}

size_t BufferView::ByteStride() const
{
    return _byteStride;
}

void BufferView::SetByteStride(size_t byteStride)
{
    _byteStride = byteStride;
}

GLenum BufferView::Target() const
{
    return _target;
}

void BufferView::SetTarget(GLenum target)
{
    _target = target;
}

GLenum BufferView::Usage() const
{
    return _usage;
}

void BufferView::SetUsage(GLenum usage)
{
    _usage = usage;
}
