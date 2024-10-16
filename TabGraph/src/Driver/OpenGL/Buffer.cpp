#include "Driver/OpenGL/Buffer.hpp"

#include <array>
#include <GL/glew.h>

static inline auto GLMappingMode(BufferView::MappingMode mode) {
    static const std::array<GLbitfield, (int)BufferView::MappingMode::MaxValue> MappingModeLUT = {
        GL_MAP_READ_BIT,
        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT,
        GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
    };
    return MappingModeLUT.at(int(mode));
}


static inline auto GLType(BufferView::Type type) {
    static const std::array<GLenum, (int)BufferView::Type::MaxValue> TypeLUT = {
    GL_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_TEXTURE_BUFFER
    };
    return TypeLUT.at(int(type));
}

static inline auto GLMode(BufferView::Mode mode) {
    static const std::array<GLbitfield, (int)BufferView::Mode::MaxValue> ModeLUT = {
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT | GL_CLIENT_STORAGE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT,
        0
    };
    return ModeLUT.at(int(mode));
}

const BufferView::ImplGPU::Handle BufferView::ImplGPU::GetHandle() const
{
    return _handle;
}

std::byte* BufferView::ImplGPU::GetMappingPtr()
{
    return _mappingPointer;
}

std::byte* BufferView::ImplGPU::MapRange(const BufferView& buffer, MappingMode mappingMode, size_t start, size_t end, bool invalidate)
{
    if (GetMappingMode() != MappingMode::None) {
        //this is a remap
        Unmap(buffer);
    }
    Bind(buffer.GetType());
    if (invalidate)
        _mappingPointer = (std::byte*)glMapBufferRange(GLType(buffer.GetType()), start, end, GLMappingMode(mappingMode) | GL_MAP_INVALIDATE_RANGE_BIT);
    else
        _mappingPointer = (std::byte*)glMapBufferRange(GLType(buffer.GetType()), start, end, GLMappingMode(mappingMode));
    _SetMappingMode(mappingMode);
    _SetMappingStart(start);
    _SetMappingEnd(end);
    Done(buffer.GetType());
    return _mappingPointer;
}

std::byte* BufferView::ImplGPU::Get(const BufferView& buffer, size_t index, size_t size)
{
    assert(GetMappingMode() != MappingMode::WriteOnly);
    assert(GetMappingStart() <= index && GetMappingEnd() >= index + size);
    return _mappingPointer + (index - GetMappingStart());
}

void BufferView::ImplGPU::Set(const BufferView &buffer, std::byte* data, size_t index, size_t size)
{
    assert(GetMappingMode() != MappingMode::ReadOnly);
    assert(GetMappingStart() <= index && GetMappingEnd() >= index + size);
    std::memcpy(_mappingPointer + (index - GetMappingStart()), data, size);
    if (buffer.GetMode() == Mode::Persistent) {
        _flushStart = std::min(_flushStart, index);
        _flushEnd = std::max(_flushEnd, index + size);
    }
}

void BufferView::ImplGPU::Unmap(const BufferView& buffer)
{
    Bind(buffer.GetType());
    glUnmapBuffer(GLType(buffer.GetType()));
    _SetMappingMode(MappingMode::None);
    _SetMappingStart(0);
    _SetMappingEnd(0);
    Done(buffer.GetType());
}

void BufferView::ImplGPU::FlushRange(const BufferView& buffer, size_t start, size_t end)
{
    Bind(buffer.GetType());
    glFlushMappedBufferRange(GLType(buffer.GetType()), start, end);
    Done(buffer.GetType());
}

void BufferView::ImplGPU::Load(const BufferView& buffer, std::byte* data)
{
    if (GetLoaded()) return;
    GLuint id;
    glCreateBuffers(1, &id);
    _SetHandle(id);
    Bind(buffer.GetType());
    glBufferStorage(GLType(buffer.GetType()), buffer.GetByteLength(), data, GLMode(buffer.GetMode()));
    Done(buffer.GetType());
    if (buffer.GetMode() == Mode::Persistent)
        MapRange(buffer, buffer.GetPersistentMappingMode(), 0, buffer.GetByteLength());
    _SetLoaded(true);
}

void BufferView::ImplGPU::Unload()
{
    uint32_t handle = GetHandle();
    glDeleteBuffers(1, &handle);
    _SetHandle(0);
    _SetLoaded(false);
}

void BufferView::ImplGPU::Bind(Type type)
{
    glBindBuffer(GLType(type), GetHandle());
}

void BufferView::ImplGPU::Done(Type type)
{
    BindNone(type);
}

void BufferView::ImplGPU::BindNone(Type type)
{
    glBindBuffer(GLType(type), 0);
}

void BufferView::ImplGPU::_SetHandle(uint32_t value)
{
    _handle = value;
}

BufferView::ImplGPU::Handle OpenGL::GetHandle(std::shared_ptr<BufferView> buffer)
{
    return buffer->GetImplGPU()->GetHandle();
}

void OpenGL::Bind(std::shared_ptr<BufferView> buffer, BufferView::Type type)
{
    if (buffer == nullptr)
        BufferView::ImplGPU::BindNone(type);
    else
        buffer->GetImplGPU()->Bind(buffer->GetType());
}
