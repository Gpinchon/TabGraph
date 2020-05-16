#include "Debug.hpp"
#include "Texture/TextureBuffer.hpp"
#include "Buffer/BufferHelper.hpp"

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
	if (glCheckError(Name())) throw std::runtime_error("Error while binding Texture Buffer " + std::to_string(Accessor()->GetBufferView()->GetBuffer()->Glid()));
	//Texture::load();
	glGenTextures(1, &_glid);
	if (glCheckError(Name())) throw std::runtime_error("Error while generating texture");
	glBindTexture(GL_TEXTURE_BUFFER, glid());
	if (glCheckError(Name())) throw std::runtime_error("Error while binding texture " + std::to_string(glid()) + " to target GL_TEXTURE_BUFFER");
	glTexBuffer(GL_TEXTURE_BUFFER, InternalFormat(), Accessor()->GetBufferView()->GetBuffer()->Glid());
	if (glCheckError(Name())) throw std::runtime_error("Error while setting Buffer to Texture");
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
	return _accessor;
}

void TextureBuffer::SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor)
{
	_accessor = bufferAccessor;
}