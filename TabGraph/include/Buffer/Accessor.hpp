/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-21 21:49:52
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Buffer/Iterator.hpp>
#include <Buffer/View.hpp>
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <glm/glm.hpp>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
/**
* A typed view into a bufferView
* A bufferView contains raw binary data.
* An accessor provides a typed view into a bufferView or a subset of a bufferView.
*/
template <typename T>
class Accessor : public Core::Inherit<Core::Object, Buffer::Accessor<T>> {
public:
    PROPERTY(std::shared_ptr<Buffer::View>, BufferView, nullptr);
    /**
    * @brief Is the data to be normalized by OpenGL ?
    */
    PROPERTY(bool, Normalized, false);
    /**
    * @brief Byte offset inside the Buffer::View
    */
    PROPERTY(size_t, ByteOffset, 0);
    /**
    * @brief Number of data chunks
    */
    PROPERTY(size_t, Size, 0);
    PROPERTY(T, Min, 0);
    PROPERTY(T, Max, 0);

public:
    Accessor(std::shared_ptr<Buffer::View> bufferView)
        : Inherit()
    {
        SetSize(bufferView->GetByteSize() / sizeof(T));
        SetBufferView(bufferView);
    }
    Accessor(std::shared_ptr<Buffer::View> bufferView, size_t size, size_t byteOffset = 0)
        : Accessor(bufferView)
    {
        SetSize(size);
        SetByteOffset(byteOffset);
        SetBufferView(bufferView);
    }
    /**
     * @brief Use this constructor to allocate a new Buffer::View
     * @param count : the number of data chunks
    */
    Accessor(const size_t size)
        : Accessor(std::make_shared<Buffer::View>(size * sizeof(T)))
    {
        GetBufferView()->SetType(Buffer::View::Type::Array);
    }
    auto GetTypeSize() const
    {
        return sizeof(T);
    }
    Iterator<T> begin()
    {
        const auto& bufferView { GetBufferView() };
        bufferView->Load();
        return Iterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }
    const Iterator<T> begin() const
    {
        const auto& bufferView { GetBufferView() };
        bufferView->Load();
        return Iterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }
    Iterator<T> end()
    {
        const auto& bufferView { GetBufferView() };
        bufferView->Load();
        return begin() + GetSize();
    }
    const Iterator<T> end() const
    {
        const auto& bufferView { GetBufferView() };
        bufferView->Load();
        return begin() + GetSize();
    }
    T& at(size_t index)
    {
        assert(index < GetSize());
        return *(begin() + index);
    }
    const T& at(size_t index) const
    {
        assert(index < GetSize());
        return *(begin() + index);
    }
    operator std::vector<T>() const
    {
        std::vector<T> vector;
        for (const auto& val : *this)
            vector.push_back(val);
        return vector;
    }

private:
    Accessor();
};
}
