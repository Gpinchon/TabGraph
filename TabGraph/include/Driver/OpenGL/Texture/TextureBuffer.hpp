/*
* @Author: gpinchon
* @Date:   2021-05-04 20:08:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:08:59
*/

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Texture/TextureBuffer.hpp>
#include <Driver/OpenGL/Texture/Texture.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class TextureBuffer::Impl : public Texture::Impl {
public:
    Impl(Pixel::SizedFormat internalFormat, std::shared_ptr<Buffer::Accessor> bufferAccessor);
    Impl(const Impl& other);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

    void SetBufferAccessor(std::shared_ptr<Buffer::Accessor> bufferAccessor);
    std::shared_ptr<Buffer::Accessor> GetBufferAccessor() const;

private:
    std::shared_ptr<Buffer::Accessor> _bufferAccessor;
};
}
