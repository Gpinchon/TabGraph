#include "BufferAccessor.hpp"
#include "BufferView.hpp"

BufferAccessor::BufferAccessor(GLenum componentType, size_t count, const std::string type) : Object(""),
	_componentType(componentType),
	_count(count),
	_type(type)
{

}

std::shared_ptr<BufferAccessor> BufferAccessor::Create(GLenum componentType, size_t count, const std::string type)
{
	return std::shared_ptr<BufferAccessor>(new BufferAccessor(componentType, count, type));
}

void BufferAccessor::Load()
{
	GetBufferView()->Load();
}

void BufferAccessor::Unload()
{

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

size_t BufferAccessor::TotalComponentByteSize()
{
	return ComponentSize() * ComponentByteSize();
}

size_t BufferAccessor::ComponentByteSize()
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

size_t BufferAccessor::ComponentSize()
{
	if (Type() == "SCALAR")
		return 1;
	if (Type() == "VEC2")
		return 2;
	if (Type() == "VEC3")
		return 3;
	if (Type() == "VEC4")
		return 4;
	if (Type() == "MAT2")
		return 4;
	if (Type() == "MAT3")
		return 9;
	if (Type() == "MAT4")
		return 16;
	return 0;
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

std::string BufferAccessor::Type() const
{
	return _type;
}

void BufferAccessor::SetType(const std::string &type)
{
	_type = type;
}

std::vector<double> BufferAccessor::Max() const
{
	return _max;
}

void BufferAccessor::SetMax(std::initializer_list<double>)
{

}

std::vector<double> BufferAccessor::Min() const
{
	return _min;
}

void BufferAccessor::SetMin(std::initializer_list<double>)
{

}


