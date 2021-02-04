#include "Texture/Texture2D.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"
#include "Texture/PixelUtils.hpp"
#include "Config.hpp"
#include "Debug.hpp" // for glCheckError, debugLog
//#include "Framebuffer.hpp"
//#include "Mesh/Geometry.hpp" // for Geometry
//#include "Parser/GLSL.hpp" // for GLSL
#include "Render.hpp" // for DisplayQuad
//#include "Shader/Shader.hpp" // for Shader
#include "Tools/Tools.hpp"
#include <cstring> // for memcpy
#include <glm/gtx/rotate_vector.hpp>

Texture2D::Texture2D(glm::ivec2 size, Pixel::SizedFormat internalFormat) : Texture(Texture::Type::Texture2D, internalFormat), _Size(size) {
    SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture2D::Texture2D(std::shared_ptr<Asset> image) : Texture(Texture::Type::Texture2D) {
    SetComponent(image);
    SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void Texture2D::Load() {
    if (GetLoaded())
        return;
    if (GetComponent<Asset>() == nullptr) {
        //We don't have an image to load from, just allocate on GPU
        _AllocateStorage();
        _SetLoaded(true);
        RestoreParameters();
    }
    else if (!_onBeforeRenderSlot.Connected() && !_onImageLoadedSlot.Connected()) //We're already loading/We'll upload texture on next render, calm down...
    {
        //GetComponent<Image>()->Load();
        //_UploadImage(GetComponent<Image>());
        auto imageAsset{ GetComponent<Asset>() };
        _onImageLoadedSlot = imageAsset->OnLoaded().ConnectMember(this, &Texture2D::_OnImageLoaded);
        imageAsset->LoadAsync();
        /*imageAsset->LoadAsync();
        if (imageAsset->GetLoaded())
        {
            _UploadImage(GetComponent<Asset>());
        }
        else
        {
            
            GetComponent<Image>()->LoadAsync();
        }*/
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
    /*glTextureStorage2D(
        (GLenum)GetHandle(),
        GetMipMapNbr(),
        (GLenum)GetPixelDescription().GetSizedFormat(),
        GetSize().x,
        GetSize().y);*/
    glBindTexture((GLenum)GetType(), GetHandle());
    glTexStorage2D(
        (GLenum)GetType(),
        GetMipMapNbr(),
        (GLenum)GetPixelDescription().GetSizedFormat(),
        GetSize().x,
        GetSize().y
    );
    glBindTexture((GLenum)GetType(), 0);
}

inline void Texture2D::_OnImageLoaded(std::shared_ptr<Asset> imageAsset) {
    assert(imageAsset->GetLoaded());
    if (!_onBeforeRenderSlot.Connected())
        _onBeforeRenderSlot = Render::OnBeforeRender().ConnectMember(this, &Texture2D::_OnBeforeRender);
    _onImageLoadedSlot.Disconnect();
}

inline void Texture2D::_OnBeforeRender(float) {
    _UploadImage(GetComponent<Asset>());
    _onBeforeRenderSlot.Disconnect();
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
