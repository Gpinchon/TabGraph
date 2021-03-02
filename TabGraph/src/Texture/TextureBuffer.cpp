/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:58:00
*/
#include "Texture/TextureBuffer.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Driver/OpenGL/Buffer.hpp"
#include "Debug.hpp"

TextureBuffer::TextureBuffer(Pixel::SizedFormat internalFormat, std::shared_ptr<BufferAccessor> bufferAccessor)
    : Texture(Texture::Type::TextureBuffer, internalFormat)
{
    SetAccessor(bufferAccessor);
}

void TextureBuffer::Load()
{
    if (GetLoaded())
        return;
    auto bufferView{ Accessor()->GetBufferView() };
    //assert(bufferView->GetType() == BufferView::Type::TextureBuffer);
    bufferView->Load();
    _SetHandle(Texture::Create(GetType()));
    auto handle{ uint32_t(bufferView->GetHandle()) };
    glTextureBuffer(
        GetHandle(),
        (GLenum)GetPixelDescription().GetSizedFormat(),
        handle
    );
    _SetLoaded(true);
}

std::shared_ptr<BufferAccessor> TextureBuffer::Accessor() const
{
    return GetComponent<BufferAccessor>();
}

void TextureBuffer::SetAccessor(std::shared_ptr<BufferAccessor> bufferAccessor)
{
    SetComponent<BufferAccessor>(bufferAccessor);
}