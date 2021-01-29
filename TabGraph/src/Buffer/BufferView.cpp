/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 17:47:00
*/
#include "Buffer/BufferView.hpp"
#include "Buffer/Buffer.hpp"

size_t s_bufferViewNbr = 0;

BufferView::BufferView(size_t byteLength, std::shared_ptr<Buffer> buffer, Mode mode)
    : BufferView(byteLength, mode)
{
    SetComponent(buffer);
}

BufferView::BufferView(std::shared_ptr<Buffer> buffer, Mode mode)
    : BufferView(buffer->GetByteLength(), buffer, mode)
{
}

BufferView::BufferView(size_t byteLength, Mode mode) : Component("BufferView_" + std::to_string(++s_bufferViewNbr))
{
    SetByteLength(byteLength);
    SetMode(mode);
}

std::byte* BufferView::Get(size_t index, size_t size)
{
    if (GetMappingMode() == MappingMode::WriteOnly)
        MapRange(
            MappingMode::ReadWrite,
            std::min(index, GetMappingStart()),
            std::max(index + size, GetMappingEnd()));
    else if (GetMappingMode() == MappingMode::None)
        MapRange(
            MappingMode::ReadOnly,
            index,
            index + size);
    return _mappingPointer + (index - GetMappingStart());
}

void BufferView::Set(std::byte* data, size_t index, size_t size)
{
    if (GetMappingMode() == MappingMode::ReadOnly)
        MapRange(
            MappingMode::ReadWrite,
            std::min(index, GetMappingStart()),
            std::max(index + size, GetMappingEnd()));
    else if (GetMappingMode() == MappingMode::None)
        MapRange(
            MappingMode::WriteOnly,
            index,
            index + size);
    std::memcpy(_mappingPointer + (index - GetMappingStart()), data, size);
    if (GetType() == Type::CPU || GetMode() != Mode::Persistent)
        return;
    _flushStart = std::min(_flushStart, index);
    _flushEnd = std::max(_flushEnd, index + size);
    if (!_beforeRenderSlot.Connected())
        _beforeRenderSlot = Render::OnBeforeRender().ConnectMember(this, &BufferView::_onBeforeRender);
}

std::byte* BufferView::MapRange(MappingMode mappingMode, size_t start, size_t end, bool invalidate)
{
    assert(mappingMode != MappingMode::None);
    if (mappingMode == GetMappingMode() &&
        start >= GetMappingStart() &&
        end <= GetMappingEnd())
        return _mappingPointer + (start - GetMappingStart());
    if (GetMappingMode() != MappingMode::None) {
        //this is a remap
        Unmap();
    }
    if (!GetLoaded())
        Load();
    if (GetType() == Type::CPU) {
        _mappingPointer = &_rawData.at(start);
    }
    else {
        Bind();
        if (invalidate)
            _mappingPointer = (std::byte*)glMapBufferRange((GLenum)GetType(), start, end, (GLbitfield)mappingMode | GL_MAP_INVALIDATE_RANGE_BIT);
        else
            _mappingPointer = (std::byte*)glMapBufferRange((GLenum)GetType(), start, end, (GLbitfield)mappingMode);
        BindDefault(GetType());
    }
    _SetMappingMode(mappingMode);
    _SetMappingStart(start);
    _SetMappingEnd(end);
    return _mappingPointer + (start - GetMappingStart());
}

void BufferView::Unmap()
{
    if (GetType() != Type::CPU) {
        Bind();
        glUnmapBuffer((GLenum)GetType());
        BindDefault(GetType());
        _beforeRenderSlot.Disconnect();
    }
    _mappingPointer = nullptr;
    _SetMappingMode(MappingMode::None);
    _SetMappingStart(0);
    _SetMappingEnd(0);
    _flushStart = 0;
    _flushEnd = 0;
}

void BufferView::FlushRange(size_t start, size_t end)
{
    if (GetType() == Type::CPU)
        return;
    Bind();
    glFlushMappedBufferRange((GLenum)GetType(), start, end);
    BindDefault(GetType());
}

void BufferView::Bind()
{
    assert(GetType() != Type::Unknown && GetType() != Type::CPU);
    glBindBuffer((GLenum)GetType(), GetHandle());
}

void BufferView::BindDefault(Type bufferType)
{
    assert(bufferType != Type::Unknown && bufferType != Type::CPU);
    glBindBuffer((GLenum)bufferType, 0);
}

void BufferView::Load()
{
    if (GetLoaded())
        return;
    std::byte* bufferData{ nullptr };
    auto buffer{ GetComponent<Buffer>() };
    if (buffer != nullptr) {
        buffer->Load();
        bufferData = buffer->Get(GetByteOffset());
    }
    if (GetType() == Type::CPU) {
        _rawData = std::vector<std::byte>(bufferData, bufferData + GetByteLength());
    }
    else {
        GLuint id;
        glCreateBuffers(1, &id);
        glBindBuffer((GLenum)GetType(), id);
        glBufferStorage((GLenum)GetType(), GetByteLength(), bufferData, (GLbitfield)GetMode());
        glBindBuffer((GLenum)GetType(), 0);
        SetHandle(id);
    }
    RemoveComponent<Buffer>(buffer);
        //_mappingPointer = (std::byte*)glMapBufferRange((GLenum)GetType(), 0, GetByteLength(), (GLbitfield)GetMode());
    SetLoaded(true);
    if (GetMode() == Mode::Persistent)
        _mappingPointer = MapRange(GetPersistentMappingMode(), 0, GetByteLength());
}

void BufferView::Unload()
{
    if (GetType() == Type::CPU) {
        _rawData.clear();
    }
    else {
        auto handle = GetHandle();
        glDeleteBuffers(1, &handle);
        SetHandle(0);
    }
    SetLoaded(false);
}

void BufferView::_onBeforeRender(float)
{
    FlushRange(_flushStart, _flushEnd);
    _flushStart = 0;
    _flushEnd = 0;
    _beforeRenderSlot.Disconnect();
}
