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

TextureBuffer::Impl::Impl(TextureBuffer& texture)
	: Texture::Impl(texture)
{
}

TextureBuffer::Impl::~Impl()
{
}

void TextureBuffer::Impl::Load()
{
	if (GetLoaded())
		return;
    auto &texture{ static_cast<TextureBuffer&>(_texture) };
    auto bufferView{ texture.Accessor()->GetBufferView() };
    bufferView->Load();
    _handle = OpenGL::Texture::Generate();
    Bind();
    glTexBuffer(
        OpenGL::GetEnum(Texture::Type::TextureBuffer),
        OpenGL::GetEnum(texture.GetPixelDescription().GetSizedFormat()),
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
