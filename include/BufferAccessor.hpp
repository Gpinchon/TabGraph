#pragma once

#include <variant>
#include <memory>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Object.hpp"

class BufferView;

/**
* A typed view into a bufferView
* A bufferView contains raw binary data.
* An accessor provides a typed view into a bufferView or a subset of a bufferView.
*/
class BufferAccessor : public Object
{
public:
	enum Type
	{
		Invalid = -1,
		Scalar,
		Vec2,
		Vec3,
		Vec4,
		Mat2,
		Mat3,
		Mat4,
		MaxType
	};
	static std::shared_ptr<BufferAccessor> Create(GLenum componentType, size_t count, const BufferAccessor::Type type);
	/** The BufferView. */
	std::shared_ptr<BufferView> GetBufferView() const;
	void SetBufferView(std::shared_ptr<BufferView>);
	/** The offset relative to the start of the bufferView in bytes. */
	size_t ByteOffset() const;
	void SetByteOffset(size_t);
	/** @return : ComponentByteSize * ComponentSize */
	size_t TotalComponentByteSize() const;
	/** @return : The byte size of the components. */
	size_t ComponentByteSize() const;
	/** @return : Specifies the number of components per generic vertex attribute. */
	size_t ComponentSize() const;
	/** The datatype of components in the attribute. */
	GLenum ComponentType() const;
	void SetComponentType(GLenum);
	/** Specifies whether integer data values should be normalized. */
	bool Normalized() const;
	void SetNormalized(bool);
	/** The number of attributes referenced by this accessor. */
	size_t Count() const;
	void SetCount(size_t);
	/** @return : the Accessor's type (see : BufferAccessor::Type) */
	BufferAccessor::Type GetType() const;
	/** @return : the Accessor's type corresponding to the string */
	static BufferAccessor::Type GetType(const std::string &type);
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

protected:
	BufferAccessor(GLenum componentType, size_t count, const BufferAccessor::Type type);
	BufferAccessor() = delete;
	
private:
	std::shared_ptr<BufferView> _bufferView{nullptr};
	size_t _byteOffset {0};
	GLenum _componentType {0};
	bool _normalized{false};
	size_t _count {0};
	const BufferAccessor::Type _type { Invalid };
	typedef std::variant<double, glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4> boundsVar;
	boundsVar _max {};
	boundsVar _min {};
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