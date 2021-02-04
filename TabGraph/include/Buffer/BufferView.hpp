/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 17:47:10
*/
#pragma once

#include "Component.hpp"
#include <GL/glew.h>
#include <memory>

class Asset;

/** A view into a buffer generally representing a subset of the buffer. */
class BufferView : public Component {
public:
    using Handle = uint32_t;
    enum class Type {
        Unknown = -1,
        /**
         * @brief CPU only buffer, won't be loaded to GPU
        */
        CPU = 0,
        /**
         * @brief for geometry Vertex buffer
        */
        Array = GL_ARRAY_BUFFER,
        /**
         * @brief for geometry Index buffer
        */
        ElementArray = GL_ELEMENT_ARRAY_BUFFER,
        /**
         * @brief used to write results of glReadPixels
        */
        PixelPack = GL_PIXEL_PACK_BUFFER,
        /**
         * @brief used to store texture raw data
        */
        PixelUnpack = GL_PIXEL_UNPACK_BUFFER,
        /**
         * @brief used to store texture buffer data
        */
        TextureBuffer = GL_TEXTURE_BUFFER
    };
    enum class Mode {
        Default = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT,
        Dynamic = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT | GL_CLIENT_STORAGE_BIT,
        Persistent = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT,
        Immutable = 0
    };
    enum class MappingMode {
        None = -1,
        ReadOnly = GL_MAP_READ_BIT,
        WriteOnly = GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT,
        ReadWrite = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT |  GL_MAP_FLUSH_EXPLICIT_BIT
    };
    PROPERTY(size_t, ByteLength, 0);
    PROPERTY(size_t, ByteStride, 0);
    PROPERTY(size_t, ByteOffset, 0);
    PROPERTY(Handle, Handle, 0);
    PROPERTY(Type, Type, Type::Unknown);
    PROPERTY(Mode, Mode, Mode::Default);
    PROPERTY(bool, Loaded, false);
    PROPERTY(MappingMode, PersistentMappingMode, MappingMode::None);
    READONLYPROPERTY(MappingMode, MappingMode, MappingMode::None);
    READONLYPROPERTY(size_t, MappingStart, 0);
    READONLYPROPERTY(size_t, MappingEnd, 0);

public:
    BufferView();
    /**
     * @brief creates a BufferView that will use buffer when loaded
     * when loading is done, buffer will be released with RemoveComponent
     * @param buffer the buffer to be used when loading, released on loaging
    */
    BufferView(std::shared_ptr<Asset> buffer, Mode = Mode::Default);
    BufferView(size_t byteLength, std::shared_ptr<Asset> buffer, Mode = Mode::Default);
    BufferView(size_t byteLength, Mode = Mode::Default);
    std::byte* Get(size_t index, size_t size);
    void Set(std::byte* data, size_t index, size_t size);
    std::byte* MapRange(MappingMode mappingMode, size_t start, size_t end, bool invalidate = false);
    void Unmap();
    void FlushRange(size_t start, size_t end);
    void Bind();
    static void BindDefault(Type bufferType);
    virtual void Load();
    virtual void Unload();

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return std::static_pointer_cast<Component>(Component::Create<BufferView>(*this));
    }
    void _onBeforeRender(float);
    std::byte* _mappingPointer{ nullptr };
    Signal<float>::ScoppedSlot _beforeRenderSlot;
    size_t _flushStart{ 0 };
    size_t _flushEnd{ 0 };
    std::vector<std::byte> _rawData{};
};