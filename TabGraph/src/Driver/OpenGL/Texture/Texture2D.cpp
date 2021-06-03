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

Texture2D::Impl::Impl(const Impl& other)
    : Texture::Impl(other)
    , _compressed(other._compressed)
    , _size(other._size)
    , _asset(other._asset)
{
    _type = Texture::Type::Texture2D;
}

Texture2D::Impl::Impl(const glm::ivec2& size, const Pixel::Description& pixelDesc)
	: Texture::Impl(Texture::Type::Texture2D, pixelDesc)
    , _size(size)
{
}

Texture2D::Impl::Impl(std::shared_ptr<Asset> asset)
    : Texture::Impl(Texture::Type::Texture2D)
    , _asset(asset)
{
}

void Texture2D::Impl::Load()
{
	if (GetLoaded() ||
        _imageLoadingSlot.Connected() ||
        _imageCompressionSlot.Connected())
        return;
    auto asset{ GetImage() };
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
    _imageLoadingSlot = EventsManager::On(Event::Type::AssetLoaded).Connect([this](const Event& event) {
        assert(!_loaded);
        auto& assetEvent = event.Get<Event::Asset>();
        if (assetEvent.asset != GetImage()) return;
        _UploadImage(assetEvent.asset);
        _imageLoadingSlot.Disconnect();
    });
}

void Texture2D::Impl::Unload()
{
    if (!GetLoaded())
        return;
    OpenGL::Texture::Delete(GetHandle());
    _handle = 0;
    SetLoaded(false);
}

void Texture2D::Impl::GenerateMipmap()
{
    Bind();
    glGenerateMipmap(OpenGL::GetEnum(GetType()));
    Done();
}

void Texture2D::Impl::SetSize(const glm::ivec2& size)
{
    if (GetSize() == size) return;
    _size = size;
    Unload();
    if (GetAutoMipMap()) SetMipMapNbr(MIPMAPNBR(GetSize()));
}

void Texture2D::Impl::SetCompressed(bool compressed)
{
    if (compressed == GetCompressed())
        return;
    _compressed = compressed;
    Unload();
}

void Texture2D::Impl::_AllocateStorage() {
    _handle = OpenGL::Texture::Generate();
    Bind();
    if (GetCompressed()) {
        glTexStorage2D(
            OpenGL::GetEnum(GetType()),
            GetMipMapNbr(),
            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
            GetSize().x, GetSize().y
        );
    }
    else
        glTexStorage2D(
            OpenGL::GetEnum(GetType()),
            GetMipMapNbr(),
            OpenGL::GetEnum(GetPixelDescription().GetSizedFormat()),
            GetSize().x, GetSize().y
        );
    Done();
}

#include <FasTC/CompressedImage.h>
#include <FasTC/Image.h>
#include <FasTC/TexComp.h>
#include <FasTC/CompressionFormat.h>

#include <DispatchQueue.hpp>

inline void Texture2D::Impl::_UploadImage(std::shared_ptr<Asset> imageAsset) {
    auto image{ imageAsset->GetComponent<Image>() };
    assert(image != nullptr);
    _pixelDescription = image->GetPixelDescription();
    SetSize(image->GetSize());
    if (GetCompressed() && !_imageCompressionSlot.Connected()) {
        _AllocateStorage();
        _imageCompressionBuffer.resize(CompressedImage::GetCompressedSize(
            image->GetSize().x, image->GetSize().y,
            FasTC::ECompressionFormat::eCompressionFormat_DXT5));
        _imageCompressionTaskID = DispatchQueue::ApplicationDispatchQueue().Dispatch([this] {
            auto image{ GetImage()->GetComponent<Image>() };
            auto compressionSetting = SCompressionSettings();
            compressionSetting.format = FasTC::eCompressionFormat_DXT5;
            compressionSetting.iNumCompressions = 1;
            compressionSetting.iNumThreads = 1;
            compressionSetting.iQuality = 50;
            compressionSetting.logStream = nullptr;
            CompressImageData(
                (unsigned char*)image->GetData().data(),
                image->GetSize().x, image->GetSize().y,
                (unsigned char*)_imageCompressionBuffer.data(),
                _imageCompressionBuffer.size(),
                compressionSetting
            );
        });
        _imageCompressionSlot = EventsManager::On(Event::Type::TaskComplete).Connect([this](const Event& event) {
            auto &taskEvent{ event.Get<Event::TaskComplete>() };
            if (taskEvent.taskID != _imageCompressionTaskID.taskID ||
                taskEvent.dispatchQueueID != _imageCompressionTaskID.dispatchQueueID)
                return;
            auto image{ GetImage()->GetComponent<Image>() };
            Bind();
            glCompressedTexSubImage2D(
                OpenGL::GetEnum(Texture::Type::Texture2D),
                0, 0, 0,
                image->GetSize().x, image->GetSize().y,
                GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,
                _imageCompressionBuffer.size(),
                _imageCompressionBuffer.data()
            );
            Done();
            _imageCompressionBuffer.clear();
            _imageCompressionBuffer.shrink_to_fit();
            if (GetAutoMipMap())
                GenerateMipmap();
            SetImage(nullptr);
            SetLoaded(true);
            _imageCompressionSlot.Disconnect();
        });
    }
    else {
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
        if (GetAutoMipMap())
            GenerateMipmap();
        SetImage(nullptr);
        SetLoaded(true);
    }
}
