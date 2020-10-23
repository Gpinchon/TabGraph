#pragma once
#include "MaterialExtension.hpp"

#include <glm/vec4.hpp>

class MetallicRoughness : public MaterialExtension
{
public:
	static std::shared_ptr<MetallicRoughness> Create();
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
	MetallicRoughness();
	std::shared_ptr<Texture2D> _texture_baseColor;
	std::shared_ptr<Texture2D> _texture_metallicRoughness;
	std::shared_ptr<Texture2D> _texture_roughness;
	std::shared_ptr<Texture2D> _texture_metallic;
	float _roughness{ 1 };
	float _metallic{ 1 };
	glm::vec4 _baseColor{ 1, 1, 1, 1 };
	
};
