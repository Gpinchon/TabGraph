/*
* @Author: gpinchon
* @Date:   2021-05-04 17:11:15
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:23
*/

#include "Driver/OpenGL/Texture/TextureCubemap.hpp"
#include "Driver/OpenGL/Texture/PixelUtils.hpp"
#include "Assets/Asset.hpp"
#include "Assets/Image.hpp"


TextureCubemap::Impl::Impl(TextureCubemap& texture)
    : Texture::Impl(texture)
{
}

TextureCubemap::Impl::~Impl()
{
}

void TextureCubemap::Impl::Load()
{
    if (GetLoaded())
        return;
    auto &texture{ static_cast<TextureCubemap&>(_texture) };
    auto imageAsset { _texture.GetComponent<Asset>() };
    if (imageAsset != nullptr) {
        imageAsset->Load();
        auto image { imageAsset->GetComponent<Image>() };
        assert(image != nullptr);
        texture.SetSize(glm::ivec2(std::min(image->GetSize().x, image->GetSize().y)));
        _texture.SetPixelDescription(image->GetPixelDescription());
        if (texture.GetAutoMipMap())
            texture.SetMipMapNbr(MIPMAPNBR(image->GetSize()));
        _AllocateStorage();
        std::array<std::shared_ptr<Image>, 6> sides;
        std::array<std::thread, 6> loadingThreads;
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            sides.at(sideIndex) = Component::Create<Image>(texture.GetSize(), texture.GetPixelDescription());
            loadingThreads.at(sideIndex) = std::thread(ExtractSide, image, sides.at(sideIndex), (TextureCubemap::Side)sideIndex);
        }
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            loadingThreads.at(sideIndex).join();
            auto side { sides.at(sideIndex) };
            glTextureSubImage3D(
                GetHandle(),
                0,
                0,
                0,
                sideIndex,
                side->GetSize().x,
                side->GetSize().y,
                1,
                OpenGL::GetEnum(side->GetPixelDescription().GetUnsizedFormat()),
                OpenGL::GetEnum(side->GetPixelDescription().GetType()),
                side->GetData().data());
        }
        texture.RemoveComponent(imageAsset);
        if (texture.GetAutoMipMap())
            texture.GenerateMipmap();
    } else
        _AllocateStorage();
    SetLoaded(true);
}

void TextureCubemap::Impl::Unload()
{
    if (!GetLoaded())
        return;
    OpenGL::Texture::Delete(GetHandle());
    _handle = 0;
    SetLoaded(false);
}

void TextureCubemap::Impl::GenerateMipmap()
{
    Bind();
    glGenerateMipmap(OpenGL::GetEnum(Texture::Type::TextureCubemap));
    Done();
}

void TextureCubemap::Impl::_AllocateStorage()
{
    auto &texture{ static_cast<TextureCubemap&>(_texture) };
    _handle = OpenGL::Texture::Generate();
    Bind();
    glTexStorage2D(
        OpenGL::GetEnum(Texture::Type::TextureCubemap),
        texture.GetMipMapNbr(),
        OpenGL::GetEnum(texture.GetPixelDescription().GetSizedFormat()),
        texture.GetSize().x, texture.GetSize().y
    );
    Done();
}