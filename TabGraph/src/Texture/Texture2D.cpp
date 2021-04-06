#include "Texture/Texture2D.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Texture/PixelUtils.hpp"
#include "Config.hpp"
#include "Debug.hpp" // for glCheckError, debugLog
#include "Renderer/Renderer.hpp" // for DisplayQuad
#include "Tools/Tools.hpp"

#include <cstring> // for memcpy
#include <glm/gtx/rotate_vector.hpp>

Texture2D::Texture2D(glm::ivec2 size, Pixel::SizedFormat internalFormat) : Texture(Texture::Type::Texture2D, internalFormat), _Size(size) {
    SetParameter<Texture::Parameter::MagFilter>(Texture::Filter::Linear);
    SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::Linear);
}

Texture2D::Texture2D(glm::ivec2 size, Pixel::SizedFormat internalFormat, uint8_t multiSample) : Texture(Texture::Type::Texture2DMultisample, internalFormat), _Size(size)
{
    SetMultisample(multiSample);
}

Texture2D::Texture2D(std::shared_ptr<Asset> image) : Texture(Texture::Type::Texture2D) {
    SetComponent(image);
    SetParameter<Texture::Parameter::MagFilter>(Texture::Filter::Linear);
    SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::Linear);
}

Texture2D::Texture2D(std::shared_ptr<Asset> image, uint8_t multiSample) : Texture(Texture::Type::Texture2DMultisample) {
    SetComponent(image);
    SetParameter<Texture::Parameter::MagFilter>(Texture::Filter::Linear);
    SetParameter<Texture::Parameter::MinFilter>(Texture::Filter::Linear);
}

void Texture2D::Load() {
    if (GetLoaded())
        return;
    
    auto asset{ GetComponent<Asset>() };
    
    if (asset == nullptr) {
        //We don't have an image to load from, just allocate on GPU
        _AllocateStorage();
        _SetLoaded(true);
        RestoreParameters();
    }
    else {
        auto assetLoaded{ asset->GetLoaded() };
        if (assetLoaded) {
            _UploadImage(GetComponent<Asset>());
        }
        else if (!assetLoaded) //We're already loading/We'll upload texture on next render, calm down...
        {
            auto imageAsset{ GetComponent<Asset>() };
            imageAsset->LoadAsync();
        }
    }
}

void Texture2D::SetSize(glm::ivec2 size)
{
    if (GetAutoMipMap())
        SetMipMapNbr(MIPMAPNBR(size));
    _SetSize(size);
    Unload();
}

void Texture2D::_AllocateStorage() {
    _SetHandle(Texture::Create(GetType()));
    glBindTexture((GLenum)GetType(), GetHandle());
    if (GetType() == Texture::Type::Texture2DMultisample)
        glTexStorage2DMultisample(
            (GLenum)GetType(),
            GetMultisample(),
            (GLenum)GetPixelDescription().GetSizedFormat(),
            GetSize().x,
            GetSize().y,
            true);
    else
        glTexStorage2D(
            (GLenum)GetType(),
            GetMipMapNbr(),
            (GLenum)GetPixelDescription().GetSizedFormat(),
            GetSize().x,
            GetSize().y
        );
    glBindTexture((GLenum)GetType(), 0);
}

inline void Texture2D::_UploadImage(std::shared_ptr<Asset> imageAsset) {
    auto image{ imageAsset->GetComponent<Image>() };
    assert(image != nullptr);
    SetPixelDescription(image->GetPixelDescription());
    SetSize(image->GetSize());
    if (GetAutoMipMap())
        SetMipMapNbr(MIPMAPNBR(image->GetSize()));
    _AllocateStorage();
    glTextureSubImage2D(
        GetHandle(),
        0,
        0,
        0,
        image->GetSize().x,
        image->GetSize().y,
        (GLenum)image->GetPixelDescription().GetUnsizedFormat(),
        (GLenum)image->GetPixelDescription().GetType(),
        image->GetData().data());
    /*glBindTexture((GLenum)GetType(), GetHandle());
    glTexSubImage2D(
        (GLenum)GetType(),
        0,
        0,
        0,
        image->GetSize().x,
        image->GetSize().y,
        (GLenum)image->GetPixelDescription().GetUnsizedFormat(),
        (GLenum)image->GetPixelDescription().GetType(),
        image->GetData().data());
    glBindTexture((GLenum)GetType(), 0);*/
    if (GetAutoMipMap())
        GenerateMipmap();
    RemoveComponent(imageAsset);
    _SetLoaded(true);
    RestoreParameters();
}
