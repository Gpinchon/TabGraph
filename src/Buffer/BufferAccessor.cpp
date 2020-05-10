#include "Buffer/BufferAccessor.hpp"
#include "Buffer/BufferView.hpp"
#include <algorithm>

BufferAccessor::BufferAccessor(GLenum componentType, size_t count, const BufferAccessor::Type type) : Object(""),
	_componentType(componentType),
	_count(count),
	_type(type)
{

}

std::shared_ptr<BufferAccessor> BufferAccessor::Create(GLenum componentType, size_t count, const BufferAccessor::Type type)
{
	return std::shared_ptr<BufferAccessor>(new BufferAccessor(componentType, count, type));
}

std::shared_ptr<BufferView> BufferAccessor::GetBufferView() const
{
	return _bufferView;
}

void BufferAccessor::SetBufferView(std::shared_ptr<BufferView> bufferView)
{
	_bufferView = bufferView;
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
	switch (ComponentType())
    {
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
	switch (GetType())
	{
		case Scalar :
			return 1;
		case Vec2 :
			return 2;
		case Vec3 :
			return 3;
		case Vec4 :
			return 4;
		case Mat2 :
			return 4;
		case Mat3 :
			return 9;
		case Mat4 :
			return 16;
		default :
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

BufferAccessor::Type BufferAccessor::GetType(const std::string &type)
{
	std::string lowerType(type);
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);
    if (lowerType == "scalar")
		return Scalar;
	if (lowerType == "vec2")
		return Vec2;
	if (lowerType == "vec3")
		return Vec3;
	if (lowerType == "vec4")
		return Vec4;
	if (lowerType == "mat2")
		return Mat2;
	if (lowerType == "mat3")
		return Mat3;
	if (lowerType == "mat4")
		return Mat4;
	return Invalid;
}
