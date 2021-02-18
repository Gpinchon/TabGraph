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
    GetShaderExtension()->SetStage(Component::Create<ShaderStage>(GL_FRAGMENT_SHADER, Component::Create<ShaderCode>(extensionCode, "SpecularGlossiness();")));
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Diffuse", GetDiffuse());
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Specular", GetSpecular());
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Glossiness", GetGlossiness());
}

void SpecularGlossiness::SetTextureDiffuse(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.Diffuse", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE");
    _SetTextureDiffuse(texture);
}

void SpecularGlossiness::SetTextureSpecular(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.Specular", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR");
    _SetTextureSpecular(texture);
}

void SpecularGlossiness::SetTextureGlossiness(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.Glossiness", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS");
    _SetTextureGlossiness(texture);
}

void SpecularGlossiness::SetTextureSpecularGlossiness(std::shared_ptr<Texture2D> texture)
{
    GetShaderExtension()->SetTexture("SpecularGlossinessTextures.SpecularGlossiness", texture);
    texture ? GetShaderExtension()->SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS") : GetShaderExtension()->RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS");
    _SetTextureSpecularGlossiness(texture);
}

void SpecularGlossiness::SetDiffuse(glm::vec4 value)
{
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Diffuse", value);
    _SetDiffuse(value);
}

void SpecularGlossiness::SetSpecular(glm::vec3 value)
{
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Specular", value);
    _SetSpecular(value);
}

void SpecularGlossiness::SetGlossiness(float value)
{
    GetShaderExtension()->SetUniform("SpecularGlossinessValues.Glossiness", value);
    _SetGlossiness(value);
}
