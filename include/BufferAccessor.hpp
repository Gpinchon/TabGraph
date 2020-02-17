#include <memory>
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
	static std::shared_ptr<BufferAccessor> Create(GLenum componentType, size_t count, const std::string type);
	void Load();
	void Unload();
	/** The BufferView. */
	std::shared_ptr<BufferView> GetBufferView() const;
	void SetBufferView(std::shared_ptr<BufferView>);
	/** The offset relative to the start of the bufferView in bytes. */
	size_t ByteOffset() const;
	void SetByteOffset(size_t);
	/** @return : The byte size of the components. */
	size_t ComponentByteSize();
	/** @return : Specifies the number of components per generic vertex attribute. */
	size_t ComponentSize();
	/** The datatype of components in the attribute. */
	GLenum ComponentType() const;
	void SetComponentType(GLenum);
	/** Specifies whether integer data values should be normalized. */
	bool Normalized() const;
	void SetNormalized(bool);
	/** The number of attributes referenced by this accessor. */
	size_t Count() const;
	void SetCount(size_t);
	/** Specifies if the attribute is a scalar, vector, or matrix. */
	std::string Type() const;
	void SetType(const std::string &);
	/** Maximum value of each component in this attribute. */
	std::vector<double> Max() const;
	void SetMax(std::initializer_list<double>);
	/** Minimum value of each component in this attribute. */
	std::vector<double> Min() const;
	void SetMin(std::initializer_list<double>);

protected:
	BufferAccessor(GLenum componentType, size_t count, const std::string type);
	BufferAccessor() = delete;
	
private:
	std::shared_ptr<BufferView> _bufferView{nullptr};
	size_t _byteOffset {0};
	GLenum _componentType {0};
	bool _normalized{false};
	size_t _count {0};
	std::string _type{""};
	std::vector<double> _max{};
	std::vector<double> _min{};
};