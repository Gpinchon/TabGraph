/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   Gpinchon
* @Last Modified time: 2020-08-27 17:00:49
*/
#pragma once

#include "Component.hpp"
#include <GL/glew.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <iterator>

#include "Render.hpp"

/** A buffer points to binary geometry, animation, or skins. */
class Buffer : public Component {
public:
    Buffer() = delete;
    Buffer(size_t byteLength, GLenum usage = GL_STATIC_DRAW);
    ~Buffer();
    enum BufferAccess : GLenum {
        Read = GL_READ_ONLY,
        Write = GL_WRITE_ONLY,
        ReadWrite = GL_READ_WRITE
    };
    /** The total byte length of the buffer. */
    size_t ByteLength() const;
    /** Sets the buffer's byte length and RESIZE RAW DATA !!! */
    void SetByteLength(size_t);
    /** The uri of the buffer. */
    std::filesystem::path Uri() const;
    /** Sets the URI */
    void SetUri(std::string);
    GLuint Glid() const;
    GLenum Usage() const;
    void SetUsage(GLenum);
    bool Mapped() const;
    std::byte* MappingPointer();
    //void* Map(GLenum access);
    std::byte* MapRange(size_t offset, size_t length, GLbitfield access);
    void Unmap();
    void Allocate();
    template<typename T>
    void Set(T* data, size_t index, size_t size = sizeof(T)) {
        if (_mappingPointer != nullptr) {
            std::memcpy(_mappingPointer + index, data, size);
            if (index < _flushRangeOffset)
                _flushRangeOffset = index;
            if (index + size > _flushRangeLength)
                _flushRangeLength = index + size;
            if (!_updateGPUSlot.Connected())
                _updateGPUSlot = Render::OnBeforeRender().ConnectMember(this, &Buffer::_UpdateGPU);
        }
        else
            std::memcpy(_data.data() + index, data, size);
    }
    std::byte* Get(size_t index) {
        if (_mappingPointer != nullptr)
            return _mappingPointer + index;
        else
            return _data.data() + index;
    }

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return std::static_pointer_cast<Buffer>(shared_from_this());
        //auto buffer = Component::Create<Buffer>(*this);
        /*buffer->_mapped = false;
        buffer->_mappingPointer = nullptr;
        buffer->_glid = 0;
        buffer->SetLoadedGPU(false);*/
        //return buffer;
    }
    virtual void _LoadCPU() override;
    virtual void _UnloadCPU() override;
    virtual void _LoadGPU() override;
    virtual void _UnloadGPU() override;
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float delta);
    virtual void _FixedUpdateCPU(float delta) override {};
    std::filesystem::path _uri { "" };
    GLuint _glid { 0 };
    GLenum _usage { GL_STATIC_DRAW };
    std::byte* _mappingPointer { nullptr };
    Signal<float>::ScoppedSlot _updateGPUSlot { };
    size_t _byteLength;
    std::vector<std::byte> _data;
    size_t _flushRangeOffset{ 0 };
    size_t _flushRangeLength{ 0 };
};