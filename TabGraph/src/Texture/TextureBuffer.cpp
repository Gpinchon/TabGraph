/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:58:00
*/
#include "Texture/TextureBuffer.hpp"
#include "Buffer/BufferAccessor.hpp"
#include "Debug.hpp"

TextureBuffer::TextureBuffer(Pixel::SizedFormat internalFormat, const std::shared_ptr<BufferAccessor> bufferAccessor)
    : Texture(Texture::Type::TextureBuffer, internalFormat)
{
    SetAccessor(bufferAccessor);
}

void TextureBuffer::Load()
{
    if (GetLoaded())
        return;
    //Accessor()->GetBufferView()->GetBuffer()->Load();
    Accessor()->GetBufferView()->Load();
    _SetHandle(Texture::Create(GetType()));
    glBindBuffer(GL_TEXTURE_BUFFER, Accessor()->GetBufferView()->GetHandle());
    glTextureBuffer(
        GetHandle(),
        (GLenum)GetPixelDescription().GetSizedFormat(),
        Accessor()->GetBufferView()->GetHandle());
    _SetLoaded(true);
    //Texture::load();
    //glBindTexture(GL_TEXTURE_BUFFER, GetGlid());
    //glTexBuffer(GL_TEXTURE_BUFFER, (GLenum)GetPixelDescription(), Accessor()->GetBufferView()->GetBuffer()->GetGlid());
    //glBindTexture(GL_TEXTURE_BUFFER, 0);
    //glBindBuffer(GL_TEXTURE_BUFFER, 0);
    
    //glBindTexture(target(), glid());
    //glCreateTextures(GL_TEXTURE_BUFFER, 1, &_glid);
    //if (glCheckError(Name()) throw std::runtime_error("");
    //glTextureBuffer(
    //	glid(),
    //	InternalFormat(),
    //	Accessor()->GetBufferView()->GetBuffer()->Glid()
    //);
    //if (glCheckError(Name()) throw std::runtime_error("");
}

std::shared_ptr<BufferAccessor> TextureBuffer::Accessor() const
{
    return GetComponent<BufferAccessor>();
}

void TextureBuffer::SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor)
{
    SetComponent<BufferAccessor>(bufferAccessor);
}