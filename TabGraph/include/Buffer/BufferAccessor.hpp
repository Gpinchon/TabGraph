/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 17:47:34
*/
#pragma once

#include <Component.hpp>

#include <glm/glm.hpp>
#include <memory>
#include <variant>

class BufferView;

/**
* A typed view into a bufferView
* A bufferView contains raw binary data.
* An accessor provides a typed view into a bufferView or a subset of a bufferView.
*/
class BufferAccessor : public Component {
public:
    enum class Type : uint8_t {
        Unknown = 0,
        Scalar = 1,
        Vec2 = 2,
        Vec3 = 3,
        Vec4 = 4,
        Mat2 = 4,
        Mat3 = 9,
        Mat4 = 16
    };
    enum class ComponentType {
        Unknown = -1,
        Int8,
        Uint8,
        Int16,
        Uint16,
        Uint32,
        Float32,
        MaxValue
    };
    /**
     * @brief Is the data to be normalized by OpenGL ?
    */
    PROPERTY(bool, Normalized, false);
    /**
     * @brief Byte offset inside the BufferView
    */
    PROPERTY(size_t, ByteOffset, 0);
    /**
     * @brief Number of data chunks
    */
    PROPERTY(size_t, Count, 0);
    /**
     * @brief The data type a data chunk
     * (Scalar, Vec2, Vec3...)
    */
    PROPERTY(Type, Type, Type::Unknown);
    /**
     * @brief Total size of a data chunk in octet
     * (Vec3 of Float32 == 3 * 4)
    */
    READONLYPROPERTY(uint8_t, TypeOctetsSize, 0);
    /**
     * @brief The component type of a chunk
     * (Int8, Uint8, Float32...)
    */
    PROPERTY(ComponentType, ComponentType, ComponentType::Unknown);
    /**
     * @brief The size of the component type in octets
     * (Int8 == 1, Float32 == 4)
    */
    READONLYPROPERTY(uint8_t, ComponentOctetsSize, 0);
public:
    
    BufferAccessor() = delete;
    BufferAccessor(const ComponentType componentType, const Type type, const size_t count);
    BufferAccessor(const ComponentType componentType, const Type type, std::shared_ptr<BufferView> bufferView);
    /** The BufferView. */
    std::shared_ptr<BufferView> GetBufferView() const;
    void SetBufferView(std::shared_ptr<BufferView>);
    /** @return : the maximum value for this Accessor */
    template <typename T>
    T Max() const;
    /** @arg max : the new maximum value for this accessor */
    template <typename T>
    void SetMax(const T max);
    /** @return : the minimum value for this Accessor */
    template <typename T>
    T Min() const;
    /** @arg min : the new minimum value for this accessor */
    template <typename T>
    void SetMin(const T min);

    template <typename T>
    void Set(const T val, size_t index);

    template <typename T>
    T Get(size_t index);

private:
    virtual std::shared_ptr<Component> _Clone() override {
        return Component::Create<BufferAccessor>(*this);
    }
    typedef std::variant<unsigned, int, double, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> boundsVar;
    boundsVar _max {};
    boundsVar _min {};
    std::shared_ptr<BufferView> _bufferView;
};

template <typename T>
inline T BufferAccessor::Max() const
{
    return std::get<T>(_max);
}

template <typename T>
inline void BufferAccessor::SetMax(const T max)
{
    _max = max;
}

template <typename T>
inline T BufferAccessor::Min() const
{
    return std::get<T>(_min);
}

template <typename T>
inline void BufferAccessor::SetMin(const T min)
{
    _min = min;
}

template<typename T>
inline void BufferAccessor::Set(const T val, size_t index)
{
    assert(sizeof(T) == GetTypeOctetsSize());
    auto bufferView{ GetBufferView() };
    auto byteStride{ bufferView->GetByteStride() ? bufferView->GetByteStride() : GetTypeOctetsSize() };
    bufferView->Load();
    bufferView->Set((std::byte*)&val, GetByteOffset() + (index * byteStride), sizeof(T));
}

template<typename T>
inline T BufferAccessor::Get(size_t index)
{
    assert(sizeof(T) == GetTypeOctetsSize());
    auto bufferView{ GetBufferView() };
    auto byteStride{ bufferView->GetByteStride() ? bufferView->GetByteStride() : GetTypeOctetsSize() };
    bufferView->Load();
    return *reinterpret_cast<T*>(bufferView->Get(GetByteOffset() + (index * byteStride), sizeof(T)));
}
