/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:48
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 23:05:42
*/
#include "Material/SpecularGlossiness.hpp"
#include "Texture/Texture2D.hpp"

SpecularGlossiness::SpecularGlossiness()
    : MaterialExtension("SpecularGlossiness")
{
    static auto extensionCode =
#include "specularGlossiness.frag"
        ;
    SetCode({ extensionCode, "SpecularGlossiness();" });
    //SetStage(std::make_shared<ShaderStage>(GL_FRAGMENT_SHADER, std::make_shared<ShaderCode>()));
    //SetUniform("SpecularGlossinessDiffuse", GetDiffuse());
    //SetUniform("SpecularGlossinessSpecular", GetSpecular());
    //SetUniform("SpecularGlossinessGlossiness", GetGlossiness());
}

std::shared_ptr<Textures::Texture2D> SpecularGlossiness::GetTextureDiffuse() const
{
    return GetTexture("SpecularGlossinessTextures.Diffuse");
}

std::shared_ptr<Textures::Texture2D> SpecularGlossiness::GetTextureSpecular() const
{
    return GetTexture("SpecularGlossinessTextures.Specular");
}

std::shared_ptr<Textures::Texture2D> SpecularGlossiness::GetTextureGlossiness() const
{
    return GetTexture("SpecularGlossinessTextures.Glossiness");
}

std::shared_ptr<Textures::Texture2D> SpecularGlossiness::GetTextureSpecularGlossiness() const
{
    return GetTexture("SpecularGlossinessTextures.SpecularGlossiness");
}

glm::vec3 SpecularGlossiness::GetDiffuse() const
{
    return GetColor("SpecularGlossinessDiffuse");
}

glm::vec3 SpecularGlossiness::GetSpecular() const
{
    return GetColor("SpecularGlossinessSpecular");
}

float SpecularGlossiness::GetGlossines() const
{
    return GetValue("SpecularGlossinessGlossiness");
}

float SpecularGlossiness::GetOpacity() const
{
    return GetValue("SpecularGlossinessOpacity");
}

void SpecularGlossiness::SetTextureDiffuse(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.Diffuse", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE");
}

void SpecularGlossiness::SetTextureSpecular(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.Specular", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR");
}

void SpecularGlossiness::SetTextureGlossiness(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.Glossiness", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS");
}

void SpecularGlossiness::SetTextureSpecularGlossiness(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.SpecularGlossiness", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS");
}

void SpecularGlossiness::SetDiffuse(glm::vec3 value)
{
    SetColor("SpecularGlossinessDiffuse", value);
}

void SpecularGlossiness::SetSpecular(glm::vec3 value)
{
    SetColor("SpecularGlossinessSpecular", value);
}

void SpecularGlossiness::SetGlossiness(float value)
{
    SetValue("SpecularGlossinessGlossiness", value);
}

void SpecularGlossiness::SetOpacity(float value)
{
    SetValue("SpecularGlossinessOpacity", value);
}
