#pragma once
#include "MaterialExtension.hpp"

#include <glm/vec4.hpp>

class MetallicRoughness : public MaterialExtension
{
public:
	MetallicRoughness();
	std::shared_ptr<Texture2D> TextureBaseColor() const;
	std::shared_ptr<Texture2D> TextureMetallicRoughness() const;
	std::shared_ptr<Texture2D> TextureRoughness() const;
	std::shared_ptr<Texture2D> TextureMetallic() const;
	void SetTextureBaseColor(std::shared_ptr<Texture2D>);
	void SetTextureMetallicRoughness(std::shared_ptr<Texture2D>);
	void SetTextureRoughness(std::shared_ptr<Texture2D>);
	void SetTextureMetallic(std::shared_ptr<Texture2D>);
	float Roughness() const;
	float Metallic() const;
	glm::vec4 BaseColor() const;
	void SetRoughness(float);
	void SetMetallic(float);
	void SetBaseColor(glm::vec4);;

private:
	virtual std::shared_ptr<Component> _Clone() override {
		return Component::Create<MetallicRoughness>(*this);
	}
	int64_t _texture_baseColor{ -1 };
	int64_t _texture_metallicRoughness{ -1 };
	int64_t _texture_roughness{ -1 };
	int64_t _texture_metallic{ -1 };
	float _roughness{ 1 };
	float _metallic{ 1 };
	glm::vec4 _baseColor{ 1, 1, 1, 1 };
	
};
