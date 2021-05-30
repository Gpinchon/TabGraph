/*
* @Author: gpinchon
* @Date:   2021-05-04 20:12:42
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:12:42
*/

#include "Driver/OpenGL/Texture/TextureBuffer.hpp"
#include "Driver/OpenGL/Texture/PixelUtils.hpp"
#include "Driver/OpenGL/Buffer.hpp"
#include "Buffer/BufferAccessor.hpp"

#include <GL/glew.h>

TextureBuffer::Impl::Impl(Pixel::SizedFormat internalFormat, std::shared_ptr<BufferAccessor> bufferAccessor)
	: Texture::Impl(Texture::Type::TextureBuffer, internalFormat)
    , _bufferAccessor(bufferAccessor)
{
}

TextureBuffer::Impl::Impl(const Impl& other)
    : Texture::Impl(other)
    , _bufferAccessor(other._bufferAccessor)
{
    _type = Texture::Type::TextureBuffer;
}

TextureBuffer::Impl::~Impl()
{
}

void TextureBuffer::Impl::Load()
{
	if (GetLoaded())
		return;
    //auto &texture{ static_cast<TextureBuffer&>(_texture) };
    auto bufferView{ GetBufferAccessor()->GetBufferView() };
    bufferView->Load();
    _handle = OpenGL::Texture::Generate();
    Bind();
    glTexBuffer(
        OpenGL::GetEnum(Texture::Type::TextureBuffer),
        OpenGL::GetEnum(GetPixelDescription().GetSizedFormat()),
        OpenGL::GetHandle(bufferView)
    );
    Done();
    SetLoaded(true);
}

void TextureBuffer::Impl::Unload()
{
	if (!GetLoaded())
		return;
	OpenGL::Texture::Delete(GetHandle());
    SetLoaded(false);
}

void TextureBuffer::Impl::GenerateMipmap()
{
}

void TextureBuffer::Impl::SetBufferAccessor(std::shared_ptr<BufferAccessor> bufferAccessor)
{
    if (bufferAccessor == _bufferAccessor)
        return;
    _bufferAccessor = bufferAccessor;
    Unload();
}

std::shared_ptr<BufferAccessor> TextureBuffer::Impl::GetBufferAccessor() const
{
    return _bufferAccessor;
}
