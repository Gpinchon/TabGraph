#pragma once

#include "Texture/Texture.hpp"
#include <memory>

class BufferAccessor;

class TextureBuffer : public Texture
{
	class Impl;
	friend Impl;
public:
	TextureBuffer(Pixel::SizedFormat internalFormat, std::shared_ptr<BufferAccessor> bufferAccessor);
	std::shared_ptr<BufferAccessor> Accessor() const;
	void SetAccessor(std::shared_ptr<BufferAccessor> bufferAccessor);

private:
	virtual std::shared_ptr<Component> _Clone() override {
		auto textureBuffer = Component::Create<TextureBuffer>(*this);
		textureBuffer->SetId(0);
		return textureBuffer;
	}
	//std::shared_ptr<BufferAccessor> _accessor { nullptr };
};