#include "Texture/Texture2D.hpp"
#include "Texture/PixelUtils.hpp"
#include "Assets/Asset.hpp"

//#if RENDERINGAPI == OpenGL
#include "Driver/OpenGL/Texture/Texture2D.hpp"
//#endif

Texture2D::Texture2D(const Texture2D& other)
    : Texture(other)
{
    _impl.reset(new Texture2D::Impl(*this));
    SetSize(other._Size);
}

Texture2D::Texture2D(glm::ivec2 size, Pixel::SizedFormat internalFormat)
    : Texture(Texture::Type::Texture2D, internalFormat)
{
    _impl.reset(new Texture2D::Impl(*this));
    SetSize(size);
}

Texture2D::Texture2D(std::shared_ptr<Asset> image) : Texture(Texture::Type::Texture2D)
{
    _impl.reset(new Texture2D::Impl(*this));
    SetComponent(image);
}

void Texture2D::SetSize(glm::ivec2 size)
{
    if (size == GetSize())
        return;
    _impl->Unload();
    if (GetAutoMipMap())
        SetMipMapNbr(MIPMAPNBR(size));
    _SetSize(size);
}
