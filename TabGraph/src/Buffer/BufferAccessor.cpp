/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 17:45:37
*/
#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferView.hpp"
#include <algorithm>

size_t bufferAccessorNbr = 0;

BufferAccessor::BufferAccessor(GLenum componentType, size_t count, const BufferAccessor::Type type)
    : Component("BufferAccessor_" + std::to_string(bufferAccessorNbr))
    , _componentType(componentType)
    , _count(count)
    , _type(type)
{
    bufferAccessorNbr++;
}

std::shared_ptr<BufferView> BufferAccessor::GetBufferView() const
{
    return GetComponent<BufferView>();
}

void BufferAccessor::SetBufferView(std::shared_ptr<BufferView> bufferView)
{
    SetComponent(bufferView);
}

size_t BufferAccessor::ByteOffset() const
{
    return _byteOffset;
}

void BufferAccessor::SetByteOffset(size_t byteOffset)
{
    _byteOffset = byteOffset;
}

size_t BufferAccessor::TotalComponentByteSize() const
{
    return ComponentSize() * ComponentByteSize();
}

size_t BufferAccessor::ComponentByteSize() const
{
    switch (ComponentType()) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return 2;
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        return 4;
    default:
        return 0;
    }
}

size_t BufferAccessor::ComponentSize() const
{
    switch (GetType()) {
    case Type::Scalar:
        return 1;
    case Type::Vec2:
        return 2;
    case Type::Vec3:
        return 3;
    case Type::Vec4:
        return 4;
    case Type::Mat2:
        return 4;
    case Type::Mat3:
        return 9;
    case Type::Mat4:
        return 16;
    default:
        return 0;
    }
}

GLenum BufferAccessor::ComponentType() const
{
    return _componentType;
}

void BufferAccessor::SetComponentType(GLenum componentType)
{
    _componentType = componentType;
}

bool BufferAccessor::Normalized() const
{
    return _normalized;
}

void BufferAccessor::SetNormalized(bool normalized)
{
    _normalized = normalized;
}

size_t BufferAccessor::Count() const
{
    return _count;
}

void BufferAccessor::SetCount(size_t count)
{
    _count = count;
}

BufferAccessor::Type BufferAccessor::GetType() const
{
    return _type;
}

BufferAccessor::Type BufferAccessor::GetType(const std::string& type)
{
    std::string lowerType(type);
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    if (lowerType == "scalar")
        return Type::Scalar;
    if (lowerType == "vec2")
        return Type::Vec2;
    if (lowerType == "vec3")
        return Type::Vec3;
    if (lowerType == "vec4")
        return Type::Vec4;
    if (lowerType == "mat2")
        return Type::Mat2;
    if (lowerType == "mat3")
        return Type::Mat3;
    if (lowerType == "mat4")
        return Type::Mat4;
    return Type::Invalid;
}
