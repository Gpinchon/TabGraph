/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:58:00
*/
#include <Texture/TextureBuffer.hpp>
#include <Texture/PixelUtils.hpp>
#include <Buffer/BufferAccessor.hpp>

//#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Buffer.hpp>
#include <Driver/OpenGL/Texture/TextureBuffer.hpp>
//#endif

TextureBuffer::TextureBuffer(Pixel::SizedFormat internalFormat, std::shared_ptr<BufferAccessor> bufferAccessor)
    : Texture()
{
    SetAccessor(bufferAccessor);
    _impl.reset(new TextureBuffer::Impl(internalFormat, bufferAccessor));
}

TextureBuffer::TextureBuffer(TextureBuffer& other)
    : Texture(other)
{
    _impl.reset(new TextureBuffer::Impl(static_cast<const TextureBuffer::Impl&>(*other._impl.get())));
}

std::shared_ptr<BufferAccessor> TextureBuffer::Accessor() const
{
    return GetComponent<BufferAccessor>();
}

void TextureBuffer::SetAccessor(std::shared_ptr<BufferAccessor> bufferAccessor)
{
    SetComponent<BufferAccessor>(bufferAccessor);
}
