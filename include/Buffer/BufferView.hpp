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

class Buffer;

/** A view into a buffer generally representing a subset of the buffer. */
class BufferView : public Component {
public:
    static std::shared_ptr<BufferView> Create(size_t byteLength, std::shared_ptr<Buffer> buffer);
    /** The buffer. */
    std::shared_ptr<Buffer> GetBuffer();
    void SetBuffer(std::shared_ptr<Buffer>);
    /** The offset into the buffer in bytes. */
    size_t ByteOffset() const;
    void SetByteOffset(size_t);
    /** The length of the bufferView in bytes. */
    size_t ByteLength() const;
    void SetByteLength(size_t);
    /** The stride, in bytes. */
    size_t ByteStride() const;
    void SetByteStride(size_t);
    /** The target that the GPU buffer should be bound to. */
    GLenum Target() const;
    void SetTarget(GLenum);
    /** The buffer's Opengl ID */
    //GLuint Glid() const;
    GLenum Usage() const;
    void SetUsage(GLenum);

protected:
    BufferView(size_t byteLength, std::shared_ptr<Buffer> buffer);
    BufferView() = delete;

private:
    virtual void _LoadCPU() override {};
    virtual void _UnloadCPU() override {};
    virtual void _LoadGPU() override {};
    virtual void _UnloadGPU() override {};
    virtual void _UpdateCPU(float /*delta*/) override {};
    virtual void _UpdateGPU(float /*delta*/) override {};
    virtual void _FixedUpdateCPU(float /*delta*/) override {};
    virtual void _FixedUpdateGPU(float /*delta*/) override {};
    size_t _byteOffset { 0 };
    size_t _byteLength { 0 };
    size_t _byteStride { 0 };
    GLenum _target { 0 };
    //GLuint _glid { 0 };
    GLenum _usage { GL_STATIC_DRAW };
};