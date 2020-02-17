#include <memory>
#include <GL/glew.h>
#include "Object.hpp"

class Buffer;

/** A view into a buffer generally representing a subset of the buffer. */
class BufferView : public Object
{
public:
	static std::shared_ptr<BufferView> Create(size_t byteLength, std::shared_ptr<Buffer> buffer);
	void Load();
	void Unload();
	/** The buffer. */
	std::shared_ptr<Buffer> GetBuffer();
	void SetBuffer(std::shared_ptr<Buffer>);
	/** The offset into the buffer in bytes. */
	size_t ByteOffset() const;
	void SetByteOffset(size_t);
	/** The length of the bufferView in bytes. */
	size_t ByteLength() const;
	void SetByteLength(size_t);
	/** The stride, in bytes. */
	size_t ByteStride() const;
	void SetByteStride(size_t);
	/** The target that the GPU buffer should be bound to. */
	GLenum Target() const;
	void SetTarget(GLenum);
	/** The buffer's Opengl ID */
	//GLuint Glid() const;
	GLenum Usage() const;
	void SetUsage(GLenum);

protected:
	BufferView(size_t byteLength, std::shared_ptr<Buffer> buffer);
	BufferView() = delete;

private:
	std::shared_ptr<Buffer> _buffer{nullptr};
	size_t _byteOffset {0};
	size_t _byteLength {0};
	size_t _byteStride {0};
	GLenum _target {0};
	//GLuint _glid { 0 };
	GLenum _usage {GL_STATIC_DRAW};
};