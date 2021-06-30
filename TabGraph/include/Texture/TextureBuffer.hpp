#pragma once

#include "Texture/Texture.hpp"
#include <memory>

class Buffer::Accessor;

class TextureBuffer : public Texture
{
	class Impl;
	friend Impl;
public:
	TextureBuffer(Pixel::SizedFormat internalFormat, std::shared_ptr<Buffer::Accessor> bufferAccessor);
	TextureBuffer(TextureBuffer& other);
	std::shared_ptr<Buffer::Accessor> Accessor() const;
	void SetAccessor(std::shared_ptr<Buffer::Accessor> bufferAccessor);

private:
	virtual std::shared_ptr<Component> _Clone() override {
		auto textureBuffer = Component::Create<TextureBuffer>(*this);
		return textureBuffer;
	}
	//std::shared_ptr<Buffer::Accessor> _accessor { nullptr };
};