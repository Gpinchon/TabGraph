/*
* @Author: gpinchon
* @Date:   2020-06-18 13:31:08
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 15:58:00
*/
#include "Texture/TextureBuffer.hpp"
#include "Buffer/BufferHelper.hpp"
#include "Debug.hpp"

TextureBuffer::TextureBuffer(const std::string& name, GLenum internalFormat, const std::shared_ptr<BufferAccessor> bufferAccessor)
    : Texture(name, GL_TEXTURE_BUFFER)
{
    SetAccessor(bufferAccessor);
    SetInternalFormat(internalFormat);
}

void TextureBuffer::generate_mipmap()
{
}

void TextureBuffer::load()
{
    if (is_loaded())
        return;

    Accessor()->GetBufferView()->GetBuffer()->LoadGPU();
    glBindBuffer(GL_TEXTURE_BUFFER, Accessor()->GetBufferView()->GetBuffer()->Glid());
    //Texture::load();
    glGenTextures(1, &_glid);
    glBindTexture(GL_TEXTURE_BUFFER, glid());
    glTexBuffer(GL_TEXTURE_BUFFER, InternalFormat(), Accessor()->GetBufferView()->GetBuffer()->Glid());
    glBindTexture(GL_TEXTURE_BUFFER, 0);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    _loaded = true;
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

void TextureBuffer::unload()
{
    Texture::unload();
}

std::shared_ptr<BufferAccessor> TextureBuffer::Accessor() const
{
    return GetComponent<BufferAccessor>();
}

void TextureBuffer::SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor)
{
    SetComponent<BufferAccessor>(bufferAccessor);
}