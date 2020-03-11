#pragma once

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"
#include <vector>

namespace BufferHelper {
	/** 
	 * @brief Creates a buffer accessor with BufferView and Buffer from vector
	 * @argument target : the buffer's target, see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
	 */
	template <typename T>
	std::shared_ptr<BufferAccessor> CreateAccessor(std::vector<T> bufferVector, GLenum target, bool normalized = false);
	template <typename T>
	std::shared_ptr<BufferView> CreateBufferView(std::vector<T> bufferVector, GLenum target);
	template <typename T>
	std::shared_ptr<Buffer> CreateBuffer(std::vector<T> bufferVector);
	template <typename T>
	T Get(std::shared_ptr<BufferAccessor>, size_t index);
	template <typename T>
	void Set(std::shared_ptr<BufferAccessor>, size_t index, T value);
}

template <typename T>
inline std::shared_ptr<Buffer> BufferHelper::CreateBuffer(std::vector<T> bufferVector)
{
	auto byteLength(bufferVector.size() * sizeof(T));
	auto buffer(Buffer::Create(byteLength));
	memcpy(buffer->RawData().data(), bufferVector.data(), byteLength);
	return buffer;
}

template <typename T>
inline std::shared_ptr<BufferView> BufferHelper::CreateBufferView(std::vector<T> bufferVector, GLenum target)
{
	auto buffer(BufferHelper::CreateBuffer(bufferVector));
	auto bufferView(BufferView::Create(buffer->ByteLength(), buffer));
	bufferView->SetTarget(target);
	return bufferView;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<unsigned> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_UNSIGNED_INT, bufferVector.size(), "SCALAR"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec3> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), "VEC3"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec2> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), "VEC2"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <typename T>
inline T BufferHelper::Get(std::shared_ptr<BufferAccessor> accessor, size_t index) {
	T ret = T();
	if (accessor == nullptr)
		return ret;
	if (sizeof(T) != accessor->TotalComponentByteSize())
		throw std::runtime_error(
			std::string("Accessor total byte size(") + std::to_string(accessor->TotalComponentByteSize()) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
	if (index >= accessor->Count())
		throw std::runtime_error(std::string("Index(") + std::to_string(index) + ") greater or equal to accessor Count(" + std::to_string(accessor->Count()) + ")");
	auto bufferView(accessor->GetBufferView());
	if (bufferView == nullptr)
		return ret;
	auto buffer(bufferView->GetBuffer());
	if (buffer == nullptr)
		return ret;
	auto stride(bufferView->ByteStride() ? bufferView->ByteStride() : accessor->TotalComponentByteSize());
	auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * stride);
	if (bufferIndex + accessor->TotalComponentByteSize() > buffer->RawData().size())
		throw std::runtime_error(std::string("Buffer index(") + std::to_string(bufferIndex + accessor->TotalComponentByteSize()) + ") out of bound(" + std::to_string(buffer->RawData().size()) + ")");
	memcpy(&ret, &buffer->RawData().at(bufferIndex), accessor->TotalComponentByteSize());
	return ret;
}

template <typename T>
inline void BufferHelper::Set(std::shared_ptr<BufferAccessor> accessor, size_t index, T value) {
	if (accessor == nullptr)
		return;
	auto bufferView(accessor->GetBufferView());
	if (bufferView == nullptr)
		return;
	auto buffer(bufferView->GetBuffer());
	if (buffer == nullptr)
		return;
	auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * accessor->TotalComponentByteSize() * bufferView->ByteStride());
	memcpy(&buffer->RawData().at(bufferIndex), &value, std::min(sizeof(T), accessor->TotalComponentByteSize()));
}