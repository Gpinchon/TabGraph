/*
* @Author: gpinchon
* @Date:   2021-05-02 20:50:11
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-02 20:50:11
*/

#include <Driver/OpenGL/Texture/Texture2D.hpp>
#include <Driver/OpenGL/Texture/PixelUtils.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Image.hpp>
#include <Event/EventsManager.hpp>

#include <GL/glew.h>

Texture2D::Impl::Impl(Texture2D& texture)
	: Texture::Impl(texture)
{
}

Texture2D::Impl::~Impl()
{

}

void Texture2D::Impl::Load()
{
	if (GetLoaded() || _loadedSlot.Connected())
        return;
    auto& texture{ static_cast<const Texture2D&>(_texture) };
    auto asset{ texture.GetComponent<Asset>() };
    if (asset == nullptr) {
        //We don't have an image to load from, just allocate on GPU
        _AllocateStorage();
        SetLoaded(true);
        return;
    }
    auto assetLoaded{ asset->GetLoaded() };
    if (assetLoaded) {
        _UploadImage(asset);
        return;
    }
    //asset->Load();
    //_UploadImage(asset);
    asset->LoadAsync();
    _loadedSlot = EventsManager::On(Event::Type::AssetLoaded).Connect([this](const Event& event) {
        assert(!_loaded);
        auto& assetEvent = event.Get<Event::Asset>();
        if (assetEvent.asset != _texture.GetComponent<Asset>()) return;
        _UploadImage(assetEvent.asset);
        _loadedSlot.Disconnect();
    });
}

void Texture2D::Impl::Unload()
{
    if (!GetLoaded())
        return;
    OpenGL::Texture::Delete(GetHandle());
    _handle = 0;
    SetLoaded(false);
    _allocated = false;
}

void Texture2D::Impl::GenerateMipmap()
{
    Bind();
    glGenerateMipmap(OpenGL::GetEnum(_texture.GetType()));
    Done();
}

void Texture2D::Impl::_AllocateStorage() {
    assert(!_allocated);
    auto& texture{ static_cast<const Texture2D&>(_texture) };
    auto type{ OpenGL::GetEnum(_texture.GetType()) };
    _handle = OpenGL::Texture::Generate();
    Bind();
    glTexStorage2D(
        type,
        texture.GetMipMapNbr(),
        OpenGL::GetEnum(texture.GetPixelDescription().GetSizedFormat()),
        texture.GetSize().x,
        texture.GetSize().y
    );
    Done();
    _allocated = true;
}

inline void Texture2D::Impl::_UploadImage(std::shared_ptr<Asset> imageAsset) {
    auto& texture{ static_cast<Texture2D&>(_texture) };
    auto image{ imageAsset->GetComponent<Image>() };
    assert(image != nullptr);
    texture.SetPixelDescription(image->GetPixelDescription());
    texture.SetSize(image->GetSize());
    if (texture.GetAutoMipMap())
        texture.SetMipMapNbr(MIPMAPNBR(image->GetSize()));
    _AllocateStorage();
    Bind();
    glTexSubImage2D(
        OpenGL::GetEnum(Texture::Type::Texture2D),
        0,
        0,
        0,
        image->GetSize().x,
        image->GetSize().y,
        OpenGL::GetEnum(image->GetPixelDescription().GetUnsizedFormat()),
        OpenGL::GetEnum(image->GetPixelDescription().GetType()),
        image->GetData().data()
    );
    Done();
    if (texture.GetAutoMipMap())
        GenerateMipmap();
    texture.RemoveComponent(imageAsset);
    SetLoaded(true);
}