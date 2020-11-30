#pragma once

#include "Texture/Texture.hpp"
#include <memory>

class BufferAccessor;

class TextureBuffer : public Texture
{
public:
	TextureBuffer(const std::string& name, GLenum internalFormat, const std::shared_ptr<BufferAccessor> bufferAccessor);
	virtual void load() override;
	virtual void unload() override;
	virtual void generate_mipmap() override;
	std::shared_ptr<BufferAccessor> Accessor() const;
	void SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor);

private:
	virtual std::shared_ptr<Component> _Clone() override {
		auto textureBuffer = Component::Create<TextureBuffer>(*this);
		textureBuffer->_glid = 0;
		textureBuffer->_loaded = false;
		return textureBuffer;
	}
	//std::shared_ptr<BufferAccessor> _accessor { nullptr };
};