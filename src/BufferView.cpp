#include "BufferView.hpp"
#include "Buffer.hpp"

BufferView::BufferView(size_t byteLength, std::shared_ptr<Buffer> buffer) : Object(""),
	_buffer(buffer),
	_byteLength(byteLength)
{

}

std::shared_ptr<BufferView> BufferView::Create(size_t byteLength, std::shared_ptr<Buffer> buffer)
{
	return std::shared_ptr<BufferView>(new BufferView(byteLength, buffer));
}

void BufferView::Load()
{
	GetBuffer()->Load();
	/*glCreateBuffers(1, &_glid);
	glBindBuffer(Target(), Glid());
    glBufferData(Target(), ByteLength(), &GetBuffer()->RawData().at(ByteOffset()), Usage());
    glBindBuffer(Target(), 0);*/
}

void BufferView::Unload()
{
	//glDeleteBuffers(1, &_glid);
}

/** The buffer. */
std::shared_ptr<Buffer> BufferView::GetBuffer()
{
	return _buffer;
}

void BufferView::SetBuffer(std::shared_ptr<Buffer> buffer)
{
	_buffer = buffer;
}

size_t BufferView::ByteOffset() const
{
	return _byteOffset;
}

void BufferView::SetByteOffset(size_t byteOffset)
{
	_byteOffset = byteOffset;
}

size_t BufferView::ByteLength() const
{
	return _byteLength;
}

void BufferView::SetByteLength(size_t byteLength)
{
	_byteLength = byteLength;
}

size_t BufferView::ByteStride() const
{
	return _byteStride;
}

void BufferView::SetByteStride(size_t byteStride)
{
	_byteStride = byteStride;
}

GLenum BufferView::Target() const
{
	return _target;
}

void BufferView::SetTarget(GLenum target)
{
	_target = target;
}

GLenum BufferView::Usage() const
{
	return _usage;
}

void BufferView::SetUsage(GLenum usage)
{
	_usage = usage;
}
