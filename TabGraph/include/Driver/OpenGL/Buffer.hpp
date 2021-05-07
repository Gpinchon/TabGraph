/*
* @Author: gpinchon
* @Date:   2021-02-28 11:32:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-01 20:16:14
*/
#pragma once

#include "Buffer/BufferView.hpp"
#include "Driver/OpenGL/ObjectHandle.hpp"

class BufferView::Handle : public OpenGL::ObjectHandle {
public:
    Handle(uint32_t v) : OpenGL::ObjectHandle(v) {};
};

class BufferView::ImplGPU {
    //PROPERTY(size_t, ByteLength, 0);
    //READONLYPROPERTY(Handle, Handle, 0);
    READONLYPROPERTY(MappingMode, MappingMode, MappingMode::None);
    READONLYPROPERTY(size_t, MappingStart, 0);
    READONLYPROPERTY(size_t, MappingEnd, 0);
    READONLYPROPERTY(bool, Loaded, false);

public:
    const Handle& GetHandle() const;
    std::byte* GetMappingPtr();
    std::byte* MapRange(const BufferView& buffer, MappingMode mappingMode, size_t start, size_t end, bool invalidate = false);
    std::byte* Get(const BufferView& buffer, size_t index, size_t size);
    void Set(const BufferView& buffer, std::byte* data, size_t index, size_t size);
    void Unmap(const BufferView& buffer);
    void FlushRange(const BufferView& buffer, size_t start, size_t end);
    void Bind(const BufferView& buffer);
    void Done(const BufferView& buffer);
    void Load(const BufferView& buffer, std::byte* data = nullptr);
    void Unload();
    static void BindNone(Type type);

private:
    void _SetHandle(uint32_t value);
    size_t _flushStart { 0 };
    size_t _flushEnd { 0 };
    std::byte* _mappingPointer { nullptr };
    Handle _handle { 0 };
};