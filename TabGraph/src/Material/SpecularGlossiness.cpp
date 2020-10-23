#include "Material/SpecularGlossiness.hpp"
#include "Texture/Texture2D.hpp"

SpecularGlossiness::SpecularGlossiness() : MaterialExtension("SpecularGlossiness")
{
    static auto extensionCode =
#include "specularGlossiness.frag"
        ;
    GetShaderExtension()->SetStage(ShaderStage::Create(GL_FRAGMENT_SHADER, ShaderCode::Create(extensionCode, "SpecularGlossiness();")));
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Diffuse", Diffuse());
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Specular", Specular());
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Glossiness", Glossiness());
}

std::shared_ptr<SpecularGlossiness> SpecularGlossiness::Create()
{
	return std::shared_ptr<SpecularGlossiness>(new SpecularGlossiness);
}

std::shared_ptr<Texture2D> SpecularGlossiness::TextureDiffuse() const
{
    return _texture_diffuse;
}

std::shared_ptr<Texture2D> SpecularGlossiness::TextureSpecular() const
{
    return _texture_specular;
}

std::shared_ptr<Texture2D> SpecularGlossiness::TextureGlossiness() const
{
    return _texture_glossiness;
}

std::shared_ptr<Texture2D> SpecularGlossiness::TextureSpecularGlossiness() const
{
    return _texture_specularGlossiness;
}

void SpecularGlossiness::SetTextureDiffuse(std::shared_ptr<Texture2D> texture)
{
    _texture_diffuse = texture;
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.Diffuse", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE");
}

void SpecularGlossiness::SetTextureSpecular(std::shared_ptr<Texture2D> texture)
{
    _texture_specular = texture;
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.Specular", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR");
}

void SpecularGlossiness::SetTextureGlossiness(std::shared_ptr<Texture2D> texture)
{
    _texture_glossiness = texture;
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.Glossiness", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS");

}

void SpecularGlossiness::SetTextureSpecularGlossiness(std::shared_ptr<Texture2D> texture)
{
    _texture_specularGlossiness = texture;
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.SpecularGlossiness", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS");
}

glm::vec4 SpecularGlossiness::Diffuse() const
{
    return _diffuse;
}

glm::vec3 SpecularGlossiness::Specular() const
{
    return _specular;
}

float SpecularGlossiness::Glossiness() const
{
    return _glossiness;
}

void SpecularGlossiness::SetDiffuse(glm::vec4 value)
{
    _diffuse = value;
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Diffuse", value);
}

void SpecularGlossiness::SetSpecular(glm::vec3 value)
{
    _specular = value;
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Specular", value);
}

void SpecularGlossiness::SetGlossiness(float value)
{
    _glossiness = value;
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Glossiness", value);
}
