#pragma once

#include <iterator>
#include <cstddef>

namespace TabGraph::Buffer {
template<typename T>
class Iterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using difference_type = std::ptrdiff_t;
	using value_type = T;
	using pointer = T*;
	using reference = T&;
	Iterator(std::byte* ptr, size_t byteStride = 0)
		: _ptr(ptr)
		, _byteStride(byteStride ? byteStride : sizeof(T)) {}
	reference operator*() { return *reinterpret_cast<T*>(_ptr); }
	pointer operator->() { return reinterpret_cast<T*>(_ptr); }
	Iterator& operator++() {
		_ptr += _byteStride;
		return *this;
	}
	Iterator operator++(int) {
		auto tmp = *this;
		++(tmp);
		return tmp;
	}

	Iterator& operator--() {
		_ptr -= _byteStride;
		return *this;
	}
	Iterator operator--(int) {
		auto tmp = *this;
		--(tmp);
		return tmp;
	}

	friend bool operator==(const Iterator& left, const Iterator& right) { return left._ptr == right._ptr; }
	friend bool operator!=(const Iterator& left, const Iterator& right) { return !(left._ptr == right._ptr); }
	friend Iterator operator+(const Iterator& left, size_t right) { return Iterator(left._ptr + (left._byteStride * right), left._byteStride); }
	friend Iterator operator-(const Iterator& left, size_t right) { return Iterator(left._ptr - (left._byteStride * right), left._byteStride); }

private:
	std::byte* _ptr{ nullptr };
	size_t _byteStride{ sizeof(T) };
};
}
