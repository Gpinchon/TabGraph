/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:43
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <memory>
#include <mutex>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset;
class BinaryData;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
/** A view into a buffer generally representing a subset of the buffer. */
class View : public Core::Inherit<Core::Object, Buffer::View> {
public:
    class ImplGPU;
    enum class Storage {
        /** @brief GPU only buffer, accessible through mapping functions */
        GPU,
        /** @brief CPU only buffer, won't be loaded to GPU */
        CPU
    };
    enum class Type {
        Unknown = -1,
        /** @brief for geometry Vertex buffer */
        Array,
        /** @brief for geometry Index buffer */
        ElementArray,
        /** @brief used to write results of glReadPixels */
        PixelPack,
        /** @brief used to store texture raw data */
        PixelUnpack,
        /** @brief used to store texture buffer data */
        TextureBuffer,
        MaxValue
    };
    enum class Mode {
        //Can be mapped in Read or Write, not really fast
        Default,
        //Can be mapped in Read or Write, for frequently updated buffers
        Dynamic,
        //Persistent mapping, fastest for frequently updated buffers
        Persistent,
        //Fastest for GPU-resident buffers, cannot be mapped
        Immutable,
        MaxValue
    };
    enum class MappingMode {
        None = -1,
        ReadOnly,
        WriteOnly,
        ReadWrite,
        MaxValue
    };
    struct Info {
        Info(const View& buffer)
            : type(buffer.GetType())
            , mode(buffer.GetMode())
            , persistentMappingMode(buffer.GetPersistentMappingMode())
        {
        }
        Type type;
        Mode mode;
        MappingMode persistentMappingMode;
    };
    READONLYPROPERTY(std::shared_ptr<ImplGPU>, ImplGPU, nullptr);
    READONLYPROPERTY(Storage, Storage, Storage::GPU);
    PROPERTY(MappingMode, PersistentMappingMode, MappingMode::None);
    PROPERTY(MappingMode, MappingMode, MappingMode::None);
    PROPERTY(size_t, ByteLength, 0);
    PROPERTY(size_t, ByteStride, 0);
    PROPERTY(size_t, ByteOffset, 0);
    PROPERTY(Type, Type, Type::Unknown);
    PROPERTY(Mode, Mode, Mode::Default);
    PROPERTY(std::shared_ptr<Assets::Asset>, Asset, nullptr);
    READONLYPROPERTY(bool, Loaded, false);

public:
    View();
    View(const View&);
    /**
         * @brief creates a View that will use buffer when loaded
         * when loading is done, buffer will be released with RemoveComponent
         * @param buffer the buffer to be used when loading, released on loaging
        */
    View(std::shared_ptr<Assets::Asset> buffer, Mode = Mode::Default);
    View(size_t byteLength, std::shared_ptr<Assets::Asset> buffer, Mode = Mode::Default);
    View(size_t byteLength, Mode = Mode::Default);
    std::byte* Get(size_t index, size_t size);
    void Set(std::byte* data, size_t index, size_t size);
    std::byte* MapRange(MappingMode mappingMode, size_t start, size_t end, bool invalidate = false);
    size_t GetMappingEnd();
    size_t GetMappingStart();
    void Unmap();
    void FlushRange(size_t start, size_t end);
    void Load();
    void Unload();
    void SetStorage(Storage storage);

private:
    friend ImplGPU;
    std::vector<std::byte> _rawData {};
    std::mutex _lock;
};
}
