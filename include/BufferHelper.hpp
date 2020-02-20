#include "Buffer.hpp"
#include "BufferView.hpp"
#include "BufferAccessor.hpp"
#include <vector>

namespace BufferHelper {
	/** Creates a buffer accessor with BufferView and Buffer from vector */
	template <typename T>
	std::shared_ptr<BufferAccessor> CreateAccessor(std::vector<T> bufferVector, bool normalized = false);
	template <typename T>
	std::shared_ptr<BufferView> CreateBufferView(std::vector<T> bufferVector);
	template <typename T>
	std::shared_ptr<Buffer> CreateBuffer(std::vector<T> bufferVector);
}

template <typename T>
std::shared_ptr<Buffer> BufferHelper::CreateBuffer(std::vector<T> bufferVector)
{
	auto byteLength(bufferVector.size() * sizeof(T));
	auto buffer(Buffer::Create(byteLength));
	std::copy(
		reinterpret_cast<std::byte*>(bufferVector.data()),
		reinterpret_cast<std::byte*>(bufferVector.data()) + bufferVector.size() * sizeof(glm::vec3),
		std::back_inserter(buffer->RawData())
		);
	return buffer;
}

template <typename T>
std::shared_ptr<BufferView> BufferHelper::CreateBufferView(std::vector<T> bufferVector)
{
	auto buffer(BufferHelper::CreateBuffer(bufferVector));
	return BufferView::Create(buffer->ByteLength(), buffer);	
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<unsigned> bufferVector, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector));
	auto bufferAccessor(BufferAccessor::Create(GL_UNSIGNED_INT, bufferVector.size(), "SCALAR"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec3> bufferVector, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), "VEC3"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}

template <>
inline std::shared_ptr<BufferAccessor> BufferHelper::CreateAccessor(std::vector<glm::vec2> bufferVector, bool normalized) {
	auto bufferView(BufferHelper::CreateBufferView(bufferVector));
	auto bufferAccessor(BufferAccessor::Create(GL_FLOAT, bufferVector.size(), "VEC2"));
	bufferAccessor->SetBufferView(bufferView);
	bufferAccessor->SetNormalized(normalized);
	return bufferAccessor;
}