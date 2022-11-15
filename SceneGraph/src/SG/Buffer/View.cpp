/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-07 15:25:43
*/
#include <SG/Buffer/View.hpp>
#include <SG/Buffer/Buffer.hpp>

#include <cassert>

namespace TabGraph::SG {
BufferView::BufferView()
    : Inherit()
{
    static auto s_bufferViewNbr = 0u;
    SetName("BufferView_" + std::to_string(++s_bufferViewNbr));
}

BufferView::BufferView(const BufferView& other)
    : Inherit(other)
    , _ByteLength(other._ByteLength)
    , _ByteStride(other._ByteStride)
    , _ByteOffset(other._ByteOffset)
    , _Buffer(other._Buffer)
{}

BufferView::BufferView(const size_t & a_ByteOffset, const size_t & a_ByteLength, const size_t & a_ByteStride) {
    SetBuffer(std::make_shared<Buffer>(a_ByteOffset + a_ByteLength));
    SetByteOffset(a_ByteOffset);
    SetByteLength(a_ByteLength);
    SetByteStride(a_ByteStride);
}

std::byte* BufferView::begin()
{
    return &*(GetBuffer()->begin() + GetByteOffset());
}

std::byte* BufferView::end()
{
    return &*(GetBuffer()->begin() + GetByteLength());
}

std::byte& BufferView::at(size_t index) {
    assert(index < GetByteLength());
    return *(begin() + index);
}
}