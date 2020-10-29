#pragma once

#include "Texture/Texture.hpp"
#include <memory>

class BufferAccessor;

class TextureBuffer : public Texture
{
public:
	TextureBuffer(const std::string& name);
	//static std::shared_ptr<TextureBuffer> Create(const std::string &name, const size_t byteLength);
	static std::shared_ptr<TextureBuffer> Create(const std::string &name, GLenum internalFormat, const std::shared_ptr<BufferAccessor> bufferAccessor);
	virtual void load() override;
	virtual void unload() override;
	virtual void generate_mipmap() override;
	std::shared_ptr<BufferAccessor> Accessor() const;
	void SetAccessor(const std::shared_ptr<BufferAccessor> bufferAccessor);

private:
	virtual std::shared_ptr<Component> _Clone() const override {
		return tools::make_shared<TextureBuffer>(*this);
	}
	std::shared_ptr<BufferAccessor> _accessor { nullptr };
};