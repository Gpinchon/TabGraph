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

//class BufferData;
//
//class FileDataManager {
//private:
//    std::unordered_map<std::filesystem::path, std::shared_ptr<BufferData>> _buffers;
//};
//
//#include <iostream>
//#include <fstream>

//class BufferData : public Component {
//public:
//    BufferData();
//    BufferData(std::byte* data, size_t totalByteSize);
//    BufferData(std::vector<std::byte> data);
//    ~BufferData() {
//        _fstream.close();
//        std::remove(_cachePath.string().c_str());
//    }
//    //BufferData(const std::filesystem::path &path);
//    operator std::vector<std::byte>&() {
//        return _dataVector();
//    }
//    auto &operator=(std::vector<std::byte> v) {
//        return _dataVector() = v;
//    }
//    template <typename... Params>
//    auto resize(Params&&... args) {
//        return _dataVector().resize(args...);
//    }
//    auto empty() {
//        return _dataVector().empty();
//    }
//    auto data() {
//        return _dataVector().data();
//    }
//    auto &at(size_t index) {
//        return _dataVector().at(index);
//    }
//    auto &operator[](size_t index) {
//        
//        return _dataVector()[index];
//    }
//private:
//    virtual std::shared_ptr<Component> _Clone() override {
//        //return std::static_pointer_cast<Component>(shared_from_this());
//        auto buffer = Component::Create<BufferData>(_dataVector());
//        return buffer;
//    }
//    auto _uncache() {
//        _fstream.close();
//        _fstream.open(_cachePath, std::fstream::binary | std::fstream::in);
//        _data.resize(_fstream.tellg());
//        _fstream.read((char*)_data.data(), _data.size());
//        _cached = false;
//    }
//    auto _cache() {
//        _fstream.open(_cachePath, std::fstream::binary | std::fstream::out | std::fstream::trunc);
//        _fstream.write((char*)_data.data(), _data.size());
//        _data.clear();
//        _data.shrink_to_fit();
//        _cached = true;
//    }
//    std::vector<std::byte>& _dataVector() {
//        if (_cached)
//            _uncache();
//        _lastAccessTime = std::chrono::steady_clock::now();
//        return _data;
//    }
//    bool _cached{ false };
//    std::vector<std::byte> _data;
//    //std::filesystem::path _filePath;
//    std::fstream _fstream;
//    std::filesystem::path _cachePath;
//    std::chrono::time_point<std::chrono::steady_clock> _lastAccessTime{ std::chrono::steady_clock::now() };
//    std::chrono::duration<double> _timeBeforeCaching{ 3 };
//    virtual void _LoadCPU() override {};
//    virtual void _UnloadCPU() override {};
//    virtual void _LoadGPU() override {};
//    virtual void _UnloadGPU() override {};
//    virtual void _UpdateCPU(float /*delta*/) override {};
//    virtual void _FixedUpdateCPU(float /*delta*/) override {
//        auto now = std::chrono::high_resolution_clock::now();
//        if (now - _lastAccessTime >= _timeBeforeCaching && !_cached)
//            _cache();
//    };
//};
//
//class FileBufferData : public BufferData {
//
//};

#include "Render.hpp"

/** A buffer points to binary geometry, animation, or skins. */
class Buffer : public Component {
public:
    Buffer() = delete;
    Buffer(size_t byteLength, GLenum usage = GL_STATIC_DRAW);
    /*Buffer(const Buffer& other) {
        _data.resize(other.ByteLength());
    }*/
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
    /** The buffer's raw data */
    //std::vector<std::byte>& RawData();
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
                _updateGPUSlot = Render::OnUpdate().ConnectMember(this, &Buffer::_UpdateGPU);
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