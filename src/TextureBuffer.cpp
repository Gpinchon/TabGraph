#include "Debug.hpp"
#include "BufferHelper.hpp"
#include "TextureBuffer.hpp"

TextureBuffer::TextureBuffer(const std::string &name) : Texture(name)
{

}

//std::shared_ptr<TextureBuffer> TextureBuffer::Create(const std::string &name, const size_t byteLength)
//{
//	auto texture(std::shared_ptr<TextureBuffer>(new TextureBuffer(name)));
//	texture->SetAccessor(BufferHelper::CreateAccessor<std::byte>(byteLength));
//}

std::shared_ptr<TextureBuffer> TextureBuffer::Create(const std::string &name, GLenum internalFormat, const std::shared_ptr<BufferAccessor> bufferAccessor)
{
	auto texture(std::shared_ptr<TextureBuffer>(new TextureBuffer(name)));
	texture->SetAccessor(bufferAccessor);
	texture->SetInternalFormat(internalFormat);
	texture->SetTarget(GL_TEXTURE_BUFFER);
	return texture;
}

void TextureBuffer::generate_mipmap()
{

}

void TextureBuffer::load()
{
	if (is_loaded())
		return;
	
	Accessor()->GetBufferView()->GetBuffer()->LoadToGPU();
	glBindBuffer(GL_TEXTURE_BUFFER, Accessor()->GetBufferView()->GetBuffer()->Glid());
	glCheckError();
	//Texture::load();
	glGenTextures(1, &_glid);
	glCheckError();
	glBindTexture(GL_TEXTURE_BUFFER, glid());
	glCheckError();
	glTexBuffer(GL_TEXTURE_BUFFER, InternalFormat(), Accessor()->GetBufferView()->GetBuffer()->Glid());
	glCheckError();
	glBindTexture(GL_TEXTURE_BUFFER, 0);
	glCheckError();
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glCheckError();
	_loaded = true;
	//glBindTexture(target(), glid());
	//glCreateTextures(GL_TEXTURE_BUFFER, 1, &_glid);
	//glCheckError();
	//glTextureBuffer(
	//	glid(),
	//	InternalFormat(),
	//	Accessor()->GetBufferView()->GetBuffer()->Glid()
	//);
	//glCheckError();
}

void TextureBuffer::unload()
{
	Texture::unload();
}

std::shared_ptr<BufferAccessor> TextureBuffer::Accessor() const
{
	return _accessor;
}

void TextureBuffer::SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor)
{
	_accessor = bufferAccessor;
}