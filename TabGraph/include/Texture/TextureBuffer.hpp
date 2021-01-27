#pragma once

#include "Texture/Texture.hpp"
#include <memory>

class BufferAccessor;

class TextureBuffer : public Texture
{
public:
	TextureBuffer(Pixel::SizedFormat internalFormat, const std::shared_ptr<BufferAccessor> bufferAccessor);
	virtual void Load() override;
	virtual void GenerateMipmap() override {};
	std::shared_ptr<BufferAccessor> Accessor() const;
	void SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor);

private:
	virtual std::shared_ptr<Component> _Clone() override {
		auto textureBuffer = Component::Create<TextureBuffer>(*this);
		textureBuffer->SetId(0);
		textureBuffer->_SetLoaded(false);
		return textureBuffer;
	}
	//std::shared_ptr<BufferAccessor> _accessor { nullptr };
};