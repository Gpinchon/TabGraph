/*
* @Author: gpinchon
* @Date:   2021-05-04 20:08:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:08:59
*/

#include "Texture/TextureBuffer.hpp"
#include "Driver/OpenGL/Texture/Texture.hpp"

class TextureBuffer::Impl : public Texture::Impl {
public:
    Impl(Pixel::SizedFormat internalFormat, std::shared_ptr<BufferAccessor> bufferAccessor);
    Impl(const Impl& other);
    ~Impl();
    virtual void Load() override;
    virtual void Unload() override;
    virtual void GenerateMipmap() override;

    void SetBufferAccessor(std::shared_ptr<BufferAccessor> bufferAccessor);
    std::shared_ptr<BufferAccessor> GetBufferAccessor() const;

private:
    std::shared_ptr<BufferAccessor> _bufferAccessor;
};