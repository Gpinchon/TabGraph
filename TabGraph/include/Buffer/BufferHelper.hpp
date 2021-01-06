/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-27 19:59:25
*/
#pragma once

#include "Buffer.hpp"
#include "BufferAccessor.hpp"
#include "BufferView.hpp"
#include <GL/glew.h>
#include <cstring>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace BufferHelper {
    /**
         * @brief Creates a buffer accessor with BufferView and Buffer from vector
         * @argument target : the buffer's target, see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
         */
    template <typename T>
    std::shared_ptr<BufferAccessor> CreateAccessor(std::vector<T> bufferVector, GLenum target = GL_ARRAY_BUFFER, bool normalized = false, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    std::shared_ptr<BufferAccessor> CreateAccessor(size_t size, GLenum target = GL_ARRAY_BUFFER, bool normalized = false, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    std::shared_ptr<BufferView> CreateBufferView(std::vector<T> bufferVector, GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    std::shared_ptr<Buffer> CreateBuffer(std::vector<T> bufferVector, GLenum usage = GL_STATIC_DRAW);
    template <typename T>
    T& Get(std::shared_ptr<Buffer>, size_t index);
    template <typename T>
    T& Get(std::shared_ptr<BufferAccessor>, size_t index);
    template <typename T>
    void Set(std::shared_ptr<Buffer>, size_t index, T value);
    template <typename T>
    void Set(std::shared_ptr<BufferAccessor>, size_t index, T value);
    template <typename T>
    void PushBack(std::shared_ptr<BufferAccessor>, T value);
};

template <typename T>
inline std::shared_ptr<Buffer> BufferHelper::CreateBuffer(std::vector<T> bufferVector, GLenum usage)
{
    auto byteLength(bufferVector.size() * sizeof(T));
    auto buffer(Component::Create<Buffer>(byteLength, usage));
    buffer->Set(bufferVector.data(), 0, bufferVector.size());
    //std::memcpy(buffer->RawData().data(), bufferVector.data(), byteLength);
    return buffer;
}

template <typename T>
inline std::shared_ptr<BufferView> BufferHelper::CreateBufferView(std::vector<T> bufferVector, GLenum target, GLenum usage)
{
    auto buffer(BufferHelper::CreateBuffer(bufferVector, usage));
    auto bufferView(Component::Create<BufferView>(buffer->ByteLength(), buffer));
    bufferView->SetTarget(target);
    return bufferView;
}

template <typename T>
inline void FindMinMax(const std::vector<T>& vector, T& minT, T& maxT)
{
    if (vector.empty()) {
        minT = T(0);
        maxT = T(0);
        return;
    }
    minT = vector.at(0);
    maxT = vector.at(0);
    for (const auto& v : vector) {
        minT = glm::max(v, maxT);
        maxT = glm::min(v, minT);
    }
}

template <>
inline void FindMinMax(const std::vector<glm::mat4>& vector, glm::mat4& minM, glm::mat4& maxM)
{
    if (vector.empty()) {
        minM = glm::mat4(0);
        maxM = glm::mat4(0);
        return;
    }
    minM = vector.at(0);
    maxM = vector.at(vector.size() - 1);
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::mat4> bufferVector, GLenum target, bool normalized, GLenum usage)
{
    auto bufferView(BufferHelper::CreateBufferView(bufferVector, target, usage));
    auto bufferAccessor(Component::Create<BufferAccessor>(GL_FLOAT, bufferVector.size(), BufferAccessor::Type::Mat4));
    glm::mat4 minM, maxM;
    FindMinMax(bufferVector, minM, maxM);
    bufferAccessor->SetBufferView(bufferView);
    bufferAccessor->SetNormalized(normalized);
    bufferAccessor->SetMin(minM);
    bufferAccessor->SetMax(maxM);
    return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<unsigned> bufferVector, GLenum target, bool normalized, GLenum usage)
{
    auto bufferView(BufferHelper::CreateBufferView(bufferVector, target, usage));
    auto bufferAccessor(Component::Create<BufferAccessor>(GL_UNSIGNED_INT, bufferVector.size(), BufferAccessor::Type::Scalar));
    unsigned minU, maxU;
    FindMinMax(bufferVector, minU, maxU);
    bufferAccessor->SetBufferView(bufferView);
    bufferAccessor->SetNormalized(normalized);
    bufferAccessor->SetMin(minU);
    bufferAccessor->SetMax(maxU);
    return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec3> bufferVector, GLenum target, bool normalized, GLenum usage)
{
    auto bufferView(BufferHelper::CreateBufferView(bufferVector, target, usage));
    auto bufferAccessor(Component::Create<BufferAccessor>(GL_FLOAT, bufferVector.size(), BufferAccessor::Type::Vec3));
    glm::vec3 minV, maxV;
    FindMinMax(bufferVector, minV, maxV);
    bufferAccessor->SetBufferView(bufferView);
    bufferAccessor->SetNormalized(normalized);
    bufferAccessor->SetMin(minV);
    bufferAccessor->SetMax(maxV);
    return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec2> bufferVector, GLenum target, bool normalized, GLenum usage)
{
    auto bufferView(BufferHelper::CreateBufferView(bufferVector, target, usage));
    auto bufferAccessor(Component::Create<BufferAccessor>(GL_FLOAT, bufferVector.size(), BufferAccessor::Type::Vec2));
    glm::vec2 min, max;
    FindMinMax(bufferVector, min, max);
    bufferAccessor->SetBufferView(bufferView);
    bufferAccessor->SetNormalized(normalized);
    bufferAccessor->SetMin(min);
    bufferAccessor->SetMax(max);
    return bufferAccessor;
}

template <typename T>
std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(size_t size, GLenum target, bool normalized, GLenum usage)
{
    return BufferHelper::CreateAccessor(std::vector<T>(size), target, normalized, usage);
}

template <typename T>
inline T &BufferHelper::Get(std::shared_ptr<Buffer> buffer, size_t index)
{
    assert(buffer != nullptr);
    if (index + sizeof(T) > buffer->ByteLength())
        throw std::runtime_error(std::string("Buffer index(") + std::to_string(index + sizeof(T)) + ") out of bound(" + std::to_string(buffer->ByteLength()) + ")");
    //return *reinterpret_cast<T*>(&buffer->RawData().at(index));
    return *reinterpret_cast<T*>(buffer->Get(index));
}

template <typename T>
inline T &BufferHelper::Get(std::shared_ptr<BufferAccessor> accessor, size_t index)
{
    assert(accessor != nullptr);
    if (sizeof(T) != accessor->TotalComponentByteSize())
        throw std::runtime_error(std::string(__FUNCTION__) + " Accessor total byte size(" + std::to_string(accessor->TotalComponentByteSize()) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
    if (index >= accessor->Count())
        throw std::runtime_error(std::string("Index(") + std::to_string(index) + ") greater or equal to accessor Count(" + std::to_string(accessor->Count()) + ")");
    auto bufferView(accessor->GetBufferView());
    assert(bufferView != nullptr);
    auto stride(bufferView->ByteStride() ? bufferView->ByteStride() : accessor->TotalComponentByteSize());
    auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * stride);
    return BufferHelper::Get<T>(bufferView->GetBuffer(), bufferIndex);
}
#include <iostream>
template <typename T>
void BufferHelper::Set(std::shared_ptr<Buffer> buffer, size_t index, T value)
{
    if (index + sizeof(T) > buffer->ByteLength())
        throw std::runtime_error(std::string("Buffer index(") + std::to_string(index + sizeof(T)) + ") out of bound(" + std::to_string(buffer->ByteLength()) + ")");
    buffer->Set(&value, index);
    //auto pointer(buffer->RawData().data() + index);
    /*auto currentValue = *reinterpret_cast<T*>(buffer->Get(index));
    if (value != currentValue) {
        std::memcpy(pointer, &value, sizeof(T));
        buffer->SetNeedsUpdateGPU(true);
    }*/
}

template <typename T>
inline void BufferHelper::Set(std::shared_ptr<BufferAccessor> accessor, size_t index, T value)
{
    if (accessor == nullptr)
        return;
    if (sizeof(T) != accessor->TotalComponentByteSize())
        throw std::runtime_error(std::string(__FUNCTION__) + " Accessor total byte size(" + std::to_string(accessor->TotalComponentByteSize()) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
    auto bufferView(accessor->GetBufferView());
    if (bufferView == nullptr)
        return;
    auto buffer(bufferView->GetBuffer());
    if (buffer == nullptr)
       return;
    auto stride(bufferView->ByteStride() ? bufferView->ByteStride() : accessor->TotalComponentByteSize());
    auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * stride);
    BufferHelper::Set(buffer, bufferIndex, value);
}

template <typename T>
void BufferHelper::PushBack(std::shared_ptr<BufferAccessor> accessor, T value)
{
    if (accessor == nullptr)
        return;
    if (sizeof(T) != accessor->TotalComponentByteSize())
        throw std::runtime_error(std::string(__FUNCTION__) + " Accessor total byte size(" + std::to_string(accessor->TotalComponentByteSize()) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
    auto bufferView(accessor->GetBufferView());
    if (bufferView == nullptr)
        return;
    auto buffer(bufferView->GetBuffer());
    if (buffer == nullptr)
        return;
    accessor->SetCount(accessor->Count() + 1);
    bufferView->SetByteLength(bufferView->ByteLength() + sizeof(T));
    buffer->SetByteLength(buffer->ByteLength() + sizeof(T));
    //buffer->RawData().resize(buffer->ByteLength());
    BufferHelper::Set(accessor, accessor->Count() - 1, value);
}