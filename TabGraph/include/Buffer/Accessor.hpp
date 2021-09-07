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

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
template<typename>
class TypedAccessor;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
class Accessor : public Core::Inherit<Core::Object, Buffer::Accessor> {
public:
    enum class ComponentType {
        Unknown = -1,
        Int8, Uint8,
        Int16, Uint16, Float16,
        Int32, Uint32, Float32,
        MaxValue
    };
    PROPERTY(size_t, ByteOffset, 0);
    PROPERTY(size_t, Size, 0);
    PROPERTY(uint8_t, ComponentTypeSize, 0);
    PROPERTY(uint8_t, ComponentNbr, 0);
    PROPERTY(ComponentType, ComponentType, ComponentType::Unknown);
    PROPERTY(std::shared_ptr<Buffer::View>, BufferView, nullptr);
    PROPERTY(bool, Normalized, false);

public:
    Accessor();
    Accessor(const std::shared_ptr<Buffer::View>& bufferView, const size_t& byteOffset, const size_t& size, const ComponentType& componentType, const uint8_t& componentsNbr)
        : Accessor()
    {
        SetBufferView(bufferView);
        SetByteOffset(byteOffset);
        SetSize(size);
        SetComponentType(componentType);
        SetComponentNbr(componentsNbr);
        SetComponentTypeSize(GetComponentTypeSize(GetComponentType()));
        if (GetBufferView() == nullptr)
            SetBufferView(std::make_shared<Buffer::View>(GetByteOffset() + GetDataByteSize()));
    }
    inline static uint8_t GetComponentTypeSize(const ComponentType& componentType) {
        switch (componentType)
        {
        case ComponentType::Int8:
        case ComponentType::Uint8:
            return 1;
            break;
        case ComponentType::Int16:
        case ComponentType::Uint16:
        case ComponentType::Float16:
            return 2;
            break;
        case ComponentType::Int32:
        case ComponentType::Uint32:
        case ComponentType::Float32:
            return 4;
            break;
        }
    }
    inline size_t GetDataByteSize() const {
        return size_t(GetComponentTypeSize()) * GetComponentNbr();
    }
    template<typename T>
    inline auto begin()
    {
        assert(sizeof(T) == GetDataByteSize());
        const auto& bufferView{ GetBufferView() };
        bufferView->Load();
        return Iterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }
    template<typename T>
    inline const auto begin() const
    {
        assert(sizeof(T) == GetDataByteSize());
        const auto& bufferView{ GetBufferView() };
        bufferView->Load();
        return Iterator<T>(&bufferView->at(GetByteOffset()), bufferView->GetByteStride());
    }

    template<typename T>
    inline const Iterator<T> end() const
    {
        return begin<T>() + GetSize();
    }
    template<typename T>
    inline Iterator<T> end()
    {
        return begin<T>() + GetSize();
    }
    template<typename T>
    inline const T& at(const size_t& index) const {
        assert(index < GetSize());
        return *(begin<T>() + index);
    }
    template<typename T>
    inline T& at(const size_t& index) {
        assert(index < GetSize());
        return *(begin<T>() + index);
    }
    bool empty() const {
        return GetSize() == 0;
    }

    template<typename T>
    inline operator TypedAccessor<T>() const {
        assert(GetDataByteSize() == sizeof(T));
        return TypedAccessor<T>(GetBufferView(), GetByteOffset(), GetSize());
    }
private:
    template<typename> friend class TypedAccessor;
    static size_t s_bufferAccessorNbr;
};

/**
* A typed view into a bufferView
* A bufferView contains raw binary data.
* An accessor provides a typed view into a bufferView or a subset of a bufferView.
*/
template <typename T>
class TypedAccessor : public Core::Inherit<Core::Object, Buffer::TypedAccessor<T>> {
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
    TypedAccessor()
        : Inherit()
    {
        SetName("Buffer::TypedAccessor_" + std::to_string(++Accessor::s_bufferAccessorNbr));
    }
    TypedAccessor(std::shared_ptr<Buffer::View> bufferView)
        : TypedAccessor()
    {
        SetSize(bufferView->GetByteSize() / sizeof(T));
        SetBufferView(bufferView);
    }
    TypedAccessor(const std::shared_ptr<Buffer::View>& bufferView, const size_t& byteOffset, const size_t& size)
        : TypedAccessor(bufferView)
    {
        SetSize(size);
        SetByteOffset(byteOffset);
        SetBufferView(bufferView);
    }
    /**
     * @brief Use this constructor to allocate a new Buffer::View
     * @param count : the number of data chunks
    */
    TypedAccessor(const size_t size)
        : TypedAccessor(std::make_shared<Buffer::View>(size * sizeof(T)))
    {
        GetBufferView()->SetType(Buffer::View::Type::Array);
    }
    auto GetTypeSize() const
    {
        return sizeof(T);
    }
    bool empty() const
    {
        return GetSize() == 0;
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
};
}
