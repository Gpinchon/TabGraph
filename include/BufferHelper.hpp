#pragma once

#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"
#include <vector>
#include <cstring>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace BufferHelper {
	/** 
	 * @brief Creates a buffer accessor with BufferView and Buffer from vector
	 * @argument target : the buffer's target, see https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindBuffer.xhtml
	 */
	template <typename T>
	std::shared_ptr<BufferAccessor> CreateAccessor(std::vector<T> bufferVector, GLenum target = GL_ARRAY_BUFFER, bool normalized = false);
	template <typename T>
	std::shared_ptr<BufferAccessor> CreateAccessor(size_t size, GLenum target = GL_ARRAY_BUFFER, bool normalized = false);
	template <typename T>
	std::shared_ptr<BufferView> CreateBufferView(std::vector<T> bufferVector, GLenum target = GL_ARRAY_BUFFER);
	template <typename T>
	std::shared_ptr<Buffer> CreateBuffer(std::vector<T> bufferVector);
	template <typename T>
	T Get(std::shared_ptr<Buffer>, size_t index);
	template <typename T>
	T Get(std::shared_ptr<BufferAccessor>, size_t index);
	template <typename T>
	void Set(std::shared_ptr<Buffer>, size_t index, T value);
	template <typename T>
	void Set(std::shared_ptr<BufferAccessor>, size_t index, T value);
	template <typename T>
	void PushBack(std::shared_ptr<BufferAccessor>, T value);
}

template <typename T>
inline std::shared_ptr<Buffer> BufferHelper::CreateBuffer(std::vector<T> bufferVector)
{
	auto byteLength(bufferVector.size() * sizeof(T));
	auto buffer(Buffer::Create(byteLength));
	std::memcpy(buffer->RawData().data(), bufferVector.data(), byteLength);
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

template <typename T>
inline void FindMinMax(const std::vector<T> &vector, T &min, T &max)
{
	min = vector.at(0);
	max = vector.at(0);
	for (const auto &v : vector)
	{
		max = glm::max(v, max);
		min = glm::min(v, min);
	}
}

template <>
inline void FindMinMax(const std::vector<glm::mat4> &vector, glm::mat4 &min, glm::mat4 &max)
{
	min = vector.at(0);
	max = vector.at(vector.size() - 1);
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::mat4> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), BufferAccessor::Type::Mat4));
	glm::mat4 min, max;
	FindMinMax(bufferVector, min, max);
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	bufferAccessor->SetMin(min);
	bufferAccessor->SetMax(max);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<unsigned> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_UNSIGNED_INT, bufferVector.size(), BufferAccessor::Type::Scalar));
	unsigned min, max;
	FindMinMax(bufferVector, min, max);
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	bufferAccessor->SetMin(min);
	bufferAccessor->SetMax(max);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec3> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), BufferAccessor::Type::Vec3));
	glm::vec3 min, max;
	FindMinMax(bufferVector, min, max);
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	bufferAccessor->SetMin(min);
	bufferAccessor->SetMax(max);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec2> bufferVector, GLenum target, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector, target));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), BufferAccessor::Type::Vec2));
	glm::vec2 min, max;
	FindMinMax(bufferVector, min, max);
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	bufferAccessor->SetMin(min);
	bufferAccessor->SetMax(max);
	return bufferAccessor;
}

template <typename T>
std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(size_t size, GLenum target, bool normalized)
{
	return BufferHelper::CreateAccessor(std::vector<T>(size), target, normalized);
}

template <typename T>
inline T BufferHelper::Get(std::shared_ptr<Buffer> buffer, size_t index)
{
	if (buffer == nullptr)
		return T();
	if (index + sizeof(T) > buffer->ByteLength())
		throw std::runtime_error(std::string("Buffer index(") + std::to_string(index + sizeof(T)) + ") out of bound(" + std::to_string(buffer->ByteLength()) + ")");
	auto pointer(buffer->RawData().data() + index);
	return *reinterpret_cast<T*>(pointer);
}

template <typename T>
inline T BufferHelper::Get(std::shared_ptr<BufferAccessor> accessor, size_t index) {
	
	if (accessor == nullptr)
		return T();
	if (sizeof(T) != accessor->TotalComponentByteSize())
		throw std::runtime_error(std::string(__FUNCTION__) + " Accessor total byte size(" + std::to_string(accessor->TotalComponentByteSize()) + ") different from size of " + typeid(T).name() + "(" + std::to_string(sizeof(T)) + ")");
	if (index >= accessor->Count())
		throw std::runtime_error(std::string("Index(") + std::to_string(index) + ") greater or equal to accessor Count(" + std::to_string(accessor->Count()) + ")");
	auto bufferView(accessor->GetBufferView());
	if (bufferView == nullptr)
		return T();
	auto stride(bufferView->ByteStride() ? bufferView->ByteStride() : accessor->TotalComponentByteSize());
	auto bufferIndex(accessor->ByteOffset() + bufferView->ByteOffset() + index * stride);
	return BufferHelper::Get<T>(bufferView->GetBuffer(), bufferIndex);
}

template <typename T>
void BufferHelper::Set(std::shared_ptr<Buffer> buffer, size_t index, T value)
{
	if (index + sizeof(T) > buffer->ByteLength())
		throw std::runtime_error(std::string("Buffer index(") + std::to_string(index + sizeof(T)) + ") out of bound(" + std::to_string(buffer->ByteLength()) + ")");
	auto pointer(buffer->RawData().data() + index);
	std::memcpy(pointer, &value, sizeof(T));
}

template <typename T>
inline void BufferHelper::Set(std::shared_ptr<BufferAccessor> accessor, size_t index, T value) {
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
	buffer->RawData().resize(buffer->ByteLength());
	BufferHelper::Set(accessor, accessor->Count() - 1, value);
}