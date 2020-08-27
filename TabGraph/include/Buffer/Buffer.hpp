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

/** A buffer points to binary geometry, animation, or skins. */
class Buffer : public Component {
public:
    enum BufferAccess : GLenum {
        Read = GL_READ_ONLY,
        Write = GL_WRITE_ONLY,
        ReadWrite = GL_READ_WRITE
    };
    static std::shared_ptr<Buffer> Create(size_t byteLength);
    //void UpdateGPU();
    /** Calls LoadToCPU() and LoadToGPU() */
    void Load();
    /** Reads the data from the uri into rawData */
    //void LoadToCPU();
    /**
	 * Loads the data of rawData into the VRAM
	 */
    //void LoadToGPU();
    /** Calls UnloadFromCPU() and UnloadFromGPU() */
    //void Unload();
    //void UnloadFromCPU();
    //void UnloadFromGPU();
    //bool LoadedToCPU();
    //bool LoadedToGPU();
    /** The total byte length of the buffer. */
    size_t ByteLength() const;
    /** Sets the buffer's byte length and RESIZE RAW DATA !!! */
    void SetByteLength(size_t);
    /** The buffer's raw data */
    std::vector<std::byte>& RawData();
    /** The uri of the buffer. */
    std::filesystem::path Uri() const;
    /** Sets the URI */
    void SetUri(std::string);
    GLuint Glid() const;
    GLenum Usage() const;
    void SetUsage(GLenum);
    bool Mapped() const;
    void* MappingPointer() const;
    void* Map(GLenum access);
    void* MapRange(size_t offset, size_t length, GLbitfield access);
    void Unmap();
    void Allocate();
    ~Buffer();

protected:
    Buffer(size_t byteLength);
    Buffer() = delete;

private:
    virtual void _LoadCPU() override;
    virtual void _UnloadCPU() override;
    virtual void _LoadGPU() override;
    virtual void _UnloadGPU() override;
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float delta) override;
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    std::filesystem::path _uri { "" };
    std::vector<std::byte> _rawData {};
    GLuint _glid { 0 };
    GLenum _usage { GL_STATIC_DRAW };
    size_t _byteLength { 0 };
    bool _mapped { false };
    void* _mappingPointer { nullptr };
};