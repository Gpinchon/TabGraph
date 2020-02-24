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
}

template <typename T>
std::shared_ptr<Buffer> BufferHelper::CreateBuffer(std::vector<T> bufferVector)
{
	auto byteLength(bufferVector.size() * sizeof(T));
	auto buffer(Buffer::Create(byteLength));
	memcpy(buffer->RawData().data(), bufferVector.data(), byteLength);
	return buffer;
}

template <typename T>
std::shared_ptr<BufferView> BufferHelper::CreateBufferView(std::vector<T> bufferVector, GLenum target)
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